#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Keep_Alive);

namespace
{
  class Listener {
    public:
      virtual ~Listener() = default;
      virtual int getValue() { return 4; }
  };

  /**
   * This class will receive a new Listener instance
   * from Ruby
   */
  class ListenerContainer
  {
    public:
      void addListener(Listener* listener) 
      {
        mListeners.push_back(listener);
      }

      void removeListener(Listener* listener)
      {
        auto iter = std::find(mListeners.begin(), mListeners.end(), listener);
        mListeners.erase(iter);
      }

      int process()
      {
        std::vector<Listener*>::iterator i = mListeners.begin();
        int accum = 0;
        for(; i != mListeners.end(); i++)
        {
          accum += (*i)->getValue();
        }

        return accum;
      }

      size_t listenerCount()
      { 
        return mListeners.size();
      }

    private:
      std::vector<Listener*> mListeners;
  };
}

SETUP(Keep_Alive)
{
  embed_ruby();
}

TEARDOWN(Keep_Alive)
{
  rb_gc_start();
}

TESTCASE(test_arg)
{
  define_class<Listener>("Listener")
    .define_constructor(Constructor<Listener>())
    .define_method("get_value", &Listener::getValue);

  define_class<ListenerContainer>("ListenerContainer")
    .define_constructor(Constructor<ListenerContainer>())
    .define_method("add_listener", &ListenerContainer::addListener, Arg("listener").keepAlive())
    .define_method("process", &ListenerContainer::process)
    .define_method("listener_count", &ListenerContainer::listenerCount);

  Module m = define_module("TestingModule");
  Object handler = m.module_eval("@handler = ListenerContainer.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  m.module_eval(R"EOS(class MyListener < Listener
                        end)EOS");

  m.module_eval("@handler.add_listener(MyListener.new)");

  // Without keep alive, this GC will crash the program because MyListener is no longer in scope
  rb_gc_start();

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

  // Without keep alive, this GC will crash the program because MyListener is no longer in scope
  rb_gc_start();
  m.module_eval("@handler.add_listener(Listener.new)");

  ASSERT_EQUAL(INT2NUM(2), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(8), handler.call("process").value());
}

namespace
{
  class Connection; 

  class Column
  {
  public:
    Column(Connection& connection, uint32_t index) : connection_(connection), index_(index)
    {
    }

    uint32_t index()
    {
      return this->index_;
    }

  private:
    Connection& connection_;
    uint32_t index_;
  };

  class Connection
  {
  public:
    Column getColumn(uint32_t index)
    {
      return Column(*this, index);
    }
  };
}

Object getColumn(Module& m, uint32_t index)
{
  Object connection = m.module_eval("Connection.new");
  return connection.call("getColumn", 3);
}

TESTCASE(test_return)
{
  define_class<Column>("Column")
    .define_method("index", &Column::index);

  define_class<Connection>("Connection")
    .define_constructor(Constructor<Connection>())
    .define_method("getColumn", &Connection::getColumn, Return().keepAlive());

  Module m = define_module("TestingModule");

  Object column = getColumn(m, 3);
  rb_gc_start();
  Object index = column.call("index");
  ASSERT_EQUAL(3, detail::From_Ruby<int>().convert(index));
}

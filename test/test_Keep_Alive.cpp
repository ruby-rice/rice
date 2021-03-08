#include "unittest.hpp"
#include "embed_ruby.hpp"
#include <rice/rice.hpp>

using namespace Rice;

TESTSUITE(Keep_Alive);

namespace {
  /**
   * The class we will subclass in Ruby
   */
  class Listener {
    public:
      virtual ~Listener() = default;
      virtual int getValue() { return 4; }
  };

  /**
   * This class will recieve a new Listener instance
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

  define_class<Listener>("Listener")
    .define_constructor(Constructor<Listener>())
    .define_method("get_value", &Listener::getValue);

  define_class<ListenerContainer>("ListenerContainer")
    .define_constructor(Constructor<ListenerContainer>())
    .define_method("add_listener", &ListenerContainer::addListener, Arg("listener").keepAlive())
    .define_method("process", &ListenerContainer::process)
    .define_method("listener_count", &ListenerContainer::listenerCount);
}

TESTCASE(add_ruby_object_to_container)
{
  Module m = define_module("TestingModule");
  Object handler = m.instance_eval("@handler = ListenerContainer.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  m.instance_eval(R"EOS(class MyListener < Listener
                        end)EOS");

  m.instance_eval("@handler.add_listener(MyListener.new)");

  // Without keep alive, this GC will crash the program because MyListener is no longer in scope
  rb_gc_start();

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

  // Without keep alive, this GC will crash the program because MyListener is no longer in scope
  rb_gc_start();
  m.instance_eval("@handler.add_listener(Listener.new)");

  ASSERT_EQUAL(INT2NUM(2), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(8), handler.call("process").value());
}

/**
 * The following test SEGFAULTs right now
 */
/*
TESTCASE(no_super_in_constructor_still_works)
{
  Module m = define_module("TestingModule");
  Object handler = m.instance_eval("@handler = ListenerContainer.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  // Because of this, there's a constructor but no super call
  m.instance_eval("class MyListener < Listener; def initialize; @val = 10; end; end;");
  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(2), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(8), handler.call("process").value());
}
*/

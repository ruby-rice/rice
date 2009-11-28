#include "unittest.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Exception.hpp"
#include "rice/Constructor.hpp"

using namespace Rice;

TESTSUITE(Data_Type);

/**
 * The tests here are for the feature of taking an instance
 * of a Ruby-subclass of a Rice wrapped class and passing
 * that instance back into the Rice wrapper. While that
 * might be confusing, the test code is pretty straight foward
 * to see what we're talking about.
 */

namespace {

  /**
   * The class we will subclass in Ruby
   */
  class Listener {
    public:
      Listener() {  }

      virtual ~Listener() {  }

      virtual int getValue() { return 4; }
  };

  /**
   * This class will recieve a new Listener instance
   * from Ruby
   */
  class ListenerHandler {

    public:

      ListenerHandler() {  }

      void addListener(Listener* newList) {
        mListeners.push_back(newList);
      }

      int process() {
        std::vector<Listener*>::iterator i = mListeners.begin();
        int accum = 0; 
        for(; i != mListeners.end(); i++) {
          accum += (*i)->getValue();
        }

        return accum;
      }

      int listenerCount() { return mListeners.size(); }

    private:
      std::vector<Listener*> mListeners;
  };
}

SETUP(Data_Type)
{
  ruby_init();

  define_class<Listener>("Listener")
    .define_constructor(Constructor<Listener>())
    .define_method("get_value", &Listener::getValue);

  define_class<ListenerHandler>("ListenerHandler")
    .define_constructor(Constructor<ListenerHandler>())
    .define_method("add_listener", &ListenerHandler::addListener)
    .define_method("process", &ListenerHandler::process)
    .define_method("listener_count", &ListenerHandler::listenerCount);

}

TESTCASE(can_send_ruby_instance_back_into_rice)
{
  Module m = define_module("TestingModule");
  Object handler = m.instance_eval("@handler = ListenerHandler.new");

  ASSERT_EQUAL(INT2NUM(0), handler.call("listener_count").value());

  m.instance_eval("class MyListener < Listener; end;");
  m.instance_eval("@handler.add_listener(MyListener.new)");

  ASSERT_EQUAL(INT2NUM(1), handler.call("listener_count").value());
  ASSERT_EQUAL(INT2NUM(4), handler.call("process").value());

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
  Object handler = m.instance_eval("@handler = ListenerHandler.new");

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

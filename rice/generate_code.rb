require 'ftools'

MAX_ARGS = 15

# ======================================================================
# TODO: This doesn't properly handle escaped %'s
def fill_template(template_str, args)
  result = template_str.dup
  args_not_found = args.dup
  result.gsub!(/%\(-(\w+)\)/) do |match|
    args_not_found.delete($1.intern)
    ''
  end
  result.gsub!(/%\((\w+)\)/) do |match|
    value = args[$1.intern]
    raise ArgumentError, "#{match} not found in argument hash" if value.nil?
    args_not_found.delete($1.intern)
    value
  end
  if args_not_found.size != 0 then
    args_not_found.each do |name, value|
      raise "#{name} not found in template"
    end
  end
  return result
end

GENERATED_FILE_WARNING = <<END
// This is a generated file.  DO NOT EDIT!!%(-macro_name)
END

# TODO: shouldn't include ruby.h directly
HEADER_TOP = <<END
#ifndef %(macro_name)
#define %(macro_name)

#{GENERATED_FILE_WARNING}

// This causes problems with certain C++ libraries
#undef TYPE

END
HEADER_BOTTOM = <<END

#endif // %(macro_name)

END

$filenames = []

def wrap_header(
    filename,
    namespace=nil,
    docstring=nil,
    include_ipp=false,
    head=nil,
    tail=nil,
    header_top=HEADER_TOP,
    header_bottom=HEADER_BOTTOM,
    &block)
  $filenames << filename
  File.open(filename, 'w') do |out|
    namespace_str = namespace.nil? ? '' : "#{namespace}__"
    trailer = File.basename(filename)
    macro_name = namespace_str + trailer + '_'
    macro_name.gsub!(/[.]/, '__')
    macro_name.tr!(':', '_')
    if header_top then
      out.puts fill_template(header_top,    { :macro_name => macro_name })
    end
    out.puts head if head
    namespace_wrapper = namespace ? method(:wrap_namespace) : method(:null_wrap)
    docstring_wrapper = docstring ? method(:wrap_docstring) : method(:null_wrap)
    namespace_wrapper.call(out, namespace) do
      docstring_wrapper.call(out, docstring) do
        yield out
      end
    end
    out.puts tail if tail
    out.puts "#include \"#{File.basename(filename, '.hpp')}.ipp\"" if include_ipp
    if header_bottom then
      out.puts fill_template(header_bottom, { :macro_name => macro_name })
    end
  end
end

def null_wrap(*args)
  yield(*args)
end

def wrap_docstring(out, docstring)
  out.puts "#ifdef DOXYGEN"
  out.puts docstring
  out.puts "#else"
  out.puts ""
  yield out
  out.puts "#endif // DOXYGEN"
end

def wrap_namespace(out, namespace)
  namespaces = namespace.split('::')
  namespaces.each do |namespace|
    out.print "namespace #{namespace}\n{\n\n"
  end
  yield out
  namespaces.reverse.each do |namespace|
    out.print "\n} // namespace #{namespace}\n"
  end
  out.print "\n"
end

if __FILE__ == $0 then

# ======================================================================
# TODO: Can I make protect accept a function or functor that
# doesn't return a VALUE?
docstring = <<END
/*! \\file
 *  \\brief A collection of functions (overloaded on number of
 *  arguments) for calling C functions that might raise Ruby exceptions.
 */

//! Call the C function f with arguments (arg1, arg2, ...).
/*! E.g.:
 *  \\code
 *    VALUE x = protect(rb_ary_new);
 *    protect(rb_ary_push(x, INT2NUM(42));
 *  \\endcode
 *
 *  Note that this function makes copies of all of its arguments; it
 *  does not take anything by reference.  All of the copies are const so
 *  that protect will not work if f takes a non-const
 *  reference to any of its arguments (though you can use non-const
 *  pointers).
 */
VALUE protect(Function f, T1 arg1, T2 arg2, ...);
END
ipp_template = <<END
namespace detail
{

template<typename Fun%(typenames)>
class Ruby_Function_%(j)
{
public:
  Ruby_Function_%(j)(Fun f%(member_args));
  inline VALUE operator()();
  static inline VALUE call(Ruby_Function_%(j) * f);
private:
  Fun f_;
  %(member_decls)
};

template<typename Fun%(typenames)>
inline Ruby_Function_%(j)<Fun%(types)>::
Ruby_Function_%(j)(Fun f%(member_args))
  : f_(f)%(initializers)
{ }

template<typename Fun%(typenames)>
inline VALUE Ruby_Function_%(j)<Fun%(types)>::
operator()()
{
  return f_(%(member_params));
}

template<typename Fun%(typenames)>
inline VALUE Ruby_Function_%(j)<Fun%(types)>::
call(Ruby_Function_%(j) * f)
{
  return (*f)();
}

} // namespace detail

template<typename Fun%(typenames)>
inline VALUE protect(Fun fun%(args))
{
  typedef detail::Ruby_Function_%(j)<Fun%(types)> RF;
  RF f(fun%(params));
  return detail::protect(
      RUBY_VALUE_FUNC(RF::call),
      reinterpret_cast<VALUE>(&f));
}

// ---------------------------------------------------------------------
END
hpp_template = <<END
template<typename Fun%(typenames)>
VALUE protect(Fun fun%(args));
END
ipp_head = <<END
#include "detail/protect.hpp"
END
hpp_head = <<END
#include "Object_defn.hpp"
END
ipp_filename = 'protect.ipp'
hpp_filename = 'protect.hpp'
wrap_header(ipp_filename, 'Rice', nil, false, ipp_head) do |ipp|
  wrap_header(hpp_filename, 'Rice', docstring, true, hpp_head) do |hpp|
    for j in 0..MAX_ARGS do
      t_array = (1..j).to_a
      type_list         = t_array.map { |x| "T#{x}"         }.join(', ')
      init_list         = t_array.map { |x| "t#{x}_(t#{x})" }.join(', ')
      param_list        = t_array.map { |x| "t#{x}"         }.join(', ')
      # arg_list          = t_array.map { |x| "T#{x} t#{x}"   }.join(', ')
      arg_list          = t_array.map { |x| "T#{x} const & t#{x}" }.join(', ')
      member_param_list = t_array.map { |x| "t#{x}_"        }.join(', ')
      member_decl_list  =
        # t_array.map { |x| "typename detail::Copy_Type<T#{x}>::Type const t#{x}_; " }
        t_array.map { |x| "T#{x} const & t#{x}_; " }
      add_comma = (j == 0) ? '' : ', '
      typename_list =
        (j == 0) ? '' : t_array.map { |x| "typename T#{x}" }.join(', ')
      member_arg_list =
        # t_array.map { |x| "typename detail::Copy_Type<T#{x}>::Type t#{x}" }.join(', ')
        t_array.map { |x| "T#{x} const & t#{x}" }.join(', ')
      ipp.puts fill_template(ipp_template, {
        :j             => j,
        :typenames     => add_comma + typename_list,
        :args          => add_comma + arg_list,
        :member_args   => add_comma + member_arg_list,
        :initializers  => add_comma + init_list,
        :member_params => member_param_list,
        :member_decls  => member_decl_list,
        :types         => add_comma + type_list,
        :params        => add_comma + param_list
      })
      ipp.puts ''
      hpp.puts fill_template(hpp_template, {
        :typenames     => add_comma + typename_list,
        :args          => add_comma + arg_list,
      })
      hpp.puts ''
    end
  end
end

# ======================================================================
docstring = <<END

//! Call the Ruby method specified by 'id' on object 'obj'.
/*! Pass in arguments (arg1, arg2, ...).  The arguments will be converted to
 *  Ruby objects with to_ruby<>.  The return value will automatically be
 *  converted to type Retval_T with from_ruby<>.
 *
 *  E.g.:
 *  \\code
 *    float y = x.call<float>("foo", z, 42);
 *  \\endcode
 */
template<typename Retval_T>
Retval_T call(Identifier id, T1 arg1, T2 arg2, ...) const;

//! Version of call which defaults to Object return type.
Object call(Identifier id, T1 arg1, T2 arg2, ...) const;
END
ipp_template = <<END

%(template)
inline Rice::Object Rice::Object::
call(Identifier id%(args)) const
{
  VALUE args[] = { %(convert_list) };
  return protect(rb_funcall2, value(), id, %(j), args);
}
END
hpp_template = <<END
%(template)
Object call(Identifier id%(args)) const;

END
ipp_head = <<END
#include "../protect.hpp"
#include "../to_from_ruby.hpp"
END
ipp_filename = 'detail/object_call.ipp'
hpp_filename = 'detail/object_call.hpp'
wrap_header(hpp_filename, nil, docstring, false, nil, nil, GENERATED_FILE_WARNING, nil) do |hpp|
  wrap_header(ipp_filename, nil, nil, false, ipp_head, nil, GENERATED_FILE_WARNING, nil) do |ipp|
    for j in 0..MAX_ARGS do
      t_array = (1..j).to_a
      arg_list = t_array.map { |x| ", T#{x} arg#{x}" }
      if j == 0 then
        template = ''
        convert_list = 'Qnil';
      else
        template = 'template<' + t_array.map { |x| "typename T#{x}" }.join(', ') + '>'
        convert_list = t_array.map { |x| "to_ruby(arg#{x})" }.join(', ')
      end
      ipp.puts fill_template(ipp_template, {
        :args            => arg_list,
        :convert_list    => convert_list,
        :j               => j,
        :template        => template,
      })
      ipp.puts
      hpp.puts fill_template(hpp_template, {
        :args            => arg_list,
        :template        => template,
      })
      hpp.puts
    end
  end
end

# ======================================================================
docstring = <<END
END
ipp_template = <<END
template<typename Func_T, typename Ret_T, %(typename_list)>
Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, %(typename_list)>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)>::
call(int argc, VALUE *args, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)> *)data.get();

    %(arg_def_list)

    bool hasSelf = (self && self != Qnil);

    if (hasSelf && argc == (Num_Args - 1)) {
      arg0 = from_ruby<Arg0_T>(self); %(self_arg_convert_list)
    } else if (argc == Num_Args) {
      %(arg_convert_list)
    } else {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, 
          hasSelf ? Num_Args - 1 : Num_Args);
    }

    return to_ruby(wrapper->func_(%(arg_list)));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, %(typename_list)>
Auto_Function_Wrapper<Func_T, void, %(typenames)>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, %(typename_list)>
VALUE Auto_Function_Wrapper<Func_T, void, %(typenames)>::
call(int argc, VALUE* args, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, %(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, %(typenames)> *)data.get();

    %(arg_def_list)

    bool hasSelf = (self && self != Qnil);

    if (hasSelf && argc == (Num_Args - 1)) {
      arg0 = from_ruby<Arg0_T>(self); %(self_arg_convert_list)
    } else if (argc == Num_Args) {
      %(arg_convert_list)
    } else {
      rb_raise(rb_eArgError, "wrong number of arguments (%d for %d)", argc, 
          hasSelf ? Num_Args - 1 : Num_Args);
    }

    wrapper->func_(%(arg_list));
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
END

# ======================================================================
# TODO: Should the handler copy the exception handler or keep a pointer
# to it?
hpp_template = <<END
template<typename Func_T, typename Ret_T, %(typename_list_d)>
class Auto_Function_Wrapper%(specializations)
  : public Wrapped_Function
{
public:
  // typedef Ret_T (*Func)(%(typenames));
  typedef Func_T Func;

  static const int Num_Args = %(j);

  Auto_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* args, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, %(typename_list)>
class Auto_Function_Wrapper<Func_T, void, %(typenames)>
  : public Wrapped_Function
{
public:
  // typedef void (*Func)(%(typenames));
  typedef Func_T Func;

  static const int Num_Args = %(j);

  Auto_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* args, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
END
hpp_head = <<END
#include "Exception_Handler_defn.hpp"

END
ipp_head = <<END
#include "method_data.hpp"
#include "../ruby_try_catch.hpp"
#include "../to_from_ruby.hpp"
END
ipp_tail = <<END
template<typename Func_T, typename Ret_T>
Auto_Function_Wrapper<Func_T, Ret_T>::
Auto_Function_Wrapper(
    Func func,
    Exception_Handler const * handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler ? handler : new Default_Exception_Handler)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T>
VALUE Auto_Function_Wrapper<Func_T, Ret_T>::
call()
{
  Auto_Function_Wrapper<Func_T, Ret_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T>*)data.get();

    return to_ruby<Ret_T>(wrapper->func_());
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T>
Auto_Function_Wrapper<Func_T, void>::
Auto_Function_Wrapper(
    Func func,
    Exception_Handler const * handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler ? handler : new Default_Exception_Handler)
  , arguments_(arguments)
{
}

template<typename Func_T>
VALUE Auto_Function_Wrapper<Func_T, void>::
call()
{
  Auto_Function_Wrapper<Func_T, void> * wrapper = 0;
  try
  {
    void * data = detail::method_data();
    wrapper =
      (Auto_Function_Wrapper<Func_T, void> *)data;

    wrapper->func_();
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}
END
hpp_tail = <<END

template<typename Func_T, typename Ret_T>
class Auto_Function_Wrapper<Func_T, Ret_T>
  : public Wrapped_Function
{
public:
  // typedef void (*Func)();
  typedef Func_T Func;

  static const int Num_Args = 0;

  Auto_Function_Wrapper(
      Func func,
      Exception_Handler const * handler = 0,
      Arguments* arguments = 0);

  static VALUE call();

private:
  Func func_;
  Exception_Handler const * handler_;
  Arguments* arguments_;
};

template<typename Func_T>
class Auto_Function_Wrapper<Func_T, void>
  : public Wrapped_Function
{
public:
  // typedef void (*Func)();
  typedef Func_T Func;

  static const int Num_Args = 0;

  Auto_Function_Wrapper(
      Func func,
      Exception_Handler const * handler = 0,
      Arguments* arguments = 0);

  static VALUE call();

private:
  Func func_;
  Exception_Handler const * handler_;
  Arguments* arguments_;
};
END
ipp_filename = 'detail/Auto_Function_Wrapper.ipp'
hpp_filename = 'detail/Auto_Function_Wrapper.hpp'
wrap_header(hpp_filename, 'Rice::detail', docstring, true, hpp_head) do |hpp|
  wrap_header(ipp_filename, 'Rice::detail', nil, false, ipp_head) do |ipp|
    MAX_ARGS.downto(0) do |j|
      t_array = (0..j).to_a
      arg_def_list      = t_array.map { |x| "Arg#{x}_T arg#{x};" }
      arg_list      = t_array.map { |x| "arg#{x}" }.join(', ')
      typenames     = t_array.map { |x| "Arg#{x}_T" }.join(', ')
      arg_convert_list  = t_array.map do |x|
        "arg#{x} = from_ruby<Arg#{x}_T>(args[#{x}]);"
      end
      self_arg_convert_list = (0...j).to_a.map do |x|
        n = x + 1
        "arg#{n} = from_ruby<Arg#{n}_T>(args[#{x}]);"
      end
      if j == MAX_ARGS then
        typename_list = t_array.map { |x| "typename Arg#{x}_T" }.join(', ')
        typename_list_d = t_array.map { |x| "typename Arg#{x}_T = void" }.join(', ')
        specializations = ''
      else
        typename_list = t_array.map { |x| "typename Arg#{x}_T" }.join(', ')
        typename_list_d = typename_list
        specializations = "<Func_T, Ret_T, #{typenames}>"
      end
      ipp.puts fill_template(ipp_template, {
        :arg_def_list     => arg_def_list,
        :arg_list         => arg_list,
        :typenames        => typenames,
        :typename_list    => typename_list,
        :arg_convert_list => arg_convert_list,
        :self_arg_convert_list => self_arg_convert_list,
      })
      hpp.puts fill_template(hpp_template, {
        :typenames        => typenames,
        :typename_list_d  => typename_list_d,
        :typename_list    => typename_list,
        :j                => j + 1,
        :specializations  => specializations,
      })
    end
    ipp.puts ipp_tail
    hpp.puts hpp_tail
  end
end

# ======================================================================
# TODO: Can I add another call() that works if from_ruby is defined
# for a reference to self instead of a pointer to self (and same for
# const reference and const pointer?)
docstring = <<END
END
ipp_template = <<END
template<typename Func_T, typename Ret_T, typename Self_T%(typename_list)>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Ret_T, typename Self_T%(typename_list)>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)>::
call(VALUE self%(value_args))
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)> *)data.get();
    Self_T * obj = from_ruby<Self_T *>(self);
    %(arg_convert_list)
    Func func = wrapper->func_;
    return to_ruby((*obj.*func)(%(arg_list)));
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

template<typename Func_T, typename Self_T%(typename_list)>
Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)>::
Auto_Member_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
  : Wrapped_Function(RUBY_METHOD_FUNC(call), Num_Args)
  , func_(func)
  , handler_(handler)
  , handler_guard_(&handler_)
  , arguments_(arguments)
{
}

template<typename Func_T, typename Self_T%(typename_list)>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)>::
call(VALUE self%(value_args))
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)> *)data.get();
    Self_T * obj = from_ruby<Self_T *>(self);
    %(arg_convert_list)
    Func func = wrapper->func_;
    (*obj.*func)(%(arg_list));
    return Qnil;
  }
  catch(...)
  {
    RUBY_TRY
    {
      if(wrapper)
      {
        return wrapper->handler_->handle_exception();
      }
      else
      {
        throw;
      }
    }
    RUBY_CATCH
  }
}

// ---------------------------------------------------------------------
END
hpp_template = <<END
template<typename Func_T, typename Ret_T, typename Self_T%(typename_list_d)>
class Auto_Member_Function_Wrapper%(specializations)
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = %(j);

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(VALUE self%(value_args));

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T%(typename_list)>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = %(j);

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(VALUE self%(value_args));

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
END
ipp_head = <<END
#include "method_data.hpp"
#include "../ruby_try_catch.hpp"
#include "../to_from_ruby.hpp"
#include <typeinfo>
END
ipp_filename = 'detail/Auto_Member_Function_Wrapper.ipp'
hpp_filename = 'detail/Auto_Member_Function_Wrapper.hpp'
wrap_header(hpp_filename, 'Rice::detail', docstring, true) do |hpp|
  wrap_header(ipp_filename, 'Rice::detail', nil, false, ipp_head) do |ipp|
    j = MAX_ARGS # TODO: what's the best way to iterate backward?
    while j >= 0 do
      t_array = (1..j).to_a
      value_args   = t_array.map { |x| ", VALUE ruby_arg#{x}" }
      arg_list      = t_array.map { |x| "arg#{x}" }.join(', ')
      typenames     = t_array.map { |x| ", Arg#{x}_T" }
      typenames_n   = t_array.map { |x| "Arg#{x}_T" }.join(', ')
      arg_convert_list = t_array.map do |x|
        "Arg#{x}_T arg#{x}(from_ruby<Arg#{x}_T>(ruby_arg#{x})); "
      end
      if j == MAX_ARGS then
        typename_list = t_array.map { |x| ", typename Arg#{x}_T" }.join
        typename_list_d = t_array.map { |x| ", typename Arg#{x}_T = void" }.join
        specializations = ''
      else
        typename_list = t_array.map { |x| ", typename Arg#{x}_T" }.join
        typename_list_d = typename_list
        specializations = "<Func_T, Ret_T, Self_T#{typenames}>"
      end
      ipp.puts fill_template(ipp_template, {
        :value_args       => value_args,
        :arg_list         => arg_list,
        :typenames        => typenames,
        :typename_list    => typename_list,
        :arg_convert_list => arg_convert_list,
      })
      hpp.puts fill_template(hpp_template, {
        :value_args       => value_args,
        :typenames        => typenames,
        # :typenames_n      => typenames_n,
        :typename_list    => typename_list,
        :typename_list_d  => typename_list_d,
        :j                => j,
        :specializations  => specializations,
      })
      j -= 1
    end
  end
end

docstring = <<END
END
ipp_template = <<END
template<typename Ret_T, %(typename_list)>
Wrapped_Function * wrap_function(
    Ret_T (*func)(%(typenames)),
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (*Func)(%(typenames));
  return new Auto_Function_Wrapper<Func, Ret_T, %(typenames)>(func, handler, arguments);
}

template<typename Ret_T, typename Self_T%(typename_list_no_self)>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(%(typenames_no_self_no_comma)),
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (Self_T::*Func)(%(typenames_no_self_no_comma));
  return new Auto_Member_Function_Wrapper<Func, Ret_T, Self_T%(typenames_no_self)>(func, handler, arguments);
}

template<typename Ret_T, typename Self_T%(typename_list_no_self)>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(%(typenames_no_self_no_comma)) const,
    Data_Object<Exception_Handler> handler,
    Arguments* arguments)
{
  typedef Ret_T (Self_T::*Func)(%(typenames_no_self_no_comma)) const;
  return new Auto_Member_Function_Wrapper<Func, Ret_T, Self_T%(typenames_no_self)>(func, handler, arguments);
}

// ---------------------------------------------------------------------
END
hpp_template = <<END
template<typename Ret_T, %(typename_list)>
Wrapped_Function * wrap_function(
    Ret_T (*func)(%(typenames)),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T%(typename_list_no_self)>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(%(typenames_no_self_no_comma)),
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

template<typename Ret_T, typename Self_T%(typename_list_no_self)>
Wrapped_Function * wrap_function(
    Ret_T (Self_T::*func)(%(typenames_no_self_no_comma)) const,
    Data_Object<Exception_Handler> handler = Rice::Nil,
    Arguments* arguments = 0);

// ---------------------------------------------------------------------
END
hpp_head = <<END
#include "Exception_Handler.hpp"
#include "Wrapped_Function.hpp"
#include "../Object_defn.hpp"
#include "../Data_Object.hpp"
#include "Arguments.hpp"

END
ipp_head = <<END
#include "Auto_Function_Wrapper.hpp"
#include "Auto_Member_Function_Wrapper.hpp"
END
ipp_filename = 'detail/wrap_function.ipp'
hpp_filename = 'detail/wrap_function.hpp'
wrap_header(hpp_filename, 'Rice::detail', docstring, true, hpp_head) do |hpp|
  wrap_header(ipp_filename, 'Rice::detail', nil, false, ipp_head) do |ipp|
    for j in 0..MAX_ARGS do
      t_array = (0..j).to_a
      typenames     = t_array.map { |x| "Arg#{x}_T" }.join(', ')
      typename_list = t_array.map { |x| "typename Arg#{x}_T" }.join(', ')
      t_array.shift
      typenames_no_self     = t_array.map { |x| ", Arg#{x}_T" }
      typename_list_no_self = t_array.map { |x| ", typename Arg#{x}_T" }
      typenames_no_self_no_comma = typenames_no_self.to_s.sub(', ', '')
      ipp.puts fill_template(ipp_template, {
        :typenames                  => typenames,
        :typename_list              => typename_list,
        :typenames_no_self          => typenames_no_self,
        :typename_list_no_self      => typename_list_no_self,
        :typenames_no_self_no_comma => typenames_no_self_no_comma,
      })
      hpp.puts fill_template(hpp_template, {
        :typenames                  => typenames,
        :typename_list              => typename_list,
        :typename_list_no_self      => typename_list_no_self,
        :typenames_no_self_no_comma => typenames_no_self_no_comma,
      })
    end
  end
end

# ======================================================================
# TODO: we have to implement this function in the class definition due
# to a bug in g++
void_list = (0..MAX_ARGS).to_a.map { |x| ", typename Arg#{x}_T=void" }
hpp_start = <<END
template<typename T#{void_list}>
class Constructor
{
private:
  Constructor()
  {
  }
};

END
hpp_template = <<END
template<typename T%(typename_list)>
class Constructor<T%(type_list)%(void_list)>
{
public:
  static void construct(Object self%(arg_list))
  {
    DATA_PTR(self.value()) = new T(%(arg_names));
  }
};

END

hpp_self_template = <<END
template<typename T%(typename_list)>
class Constructor<T, Object%(type_list)%(void_list)>
{
public:
  static void construct(Object self%(arg_list))
  {
    DATA_PTR(self.value()) = new T(self%(arg_names));
  }
};

END
hpp_head = <<END
#include "to_from_ruby_defn.hpp"
#include "detail/method_data.hpp"
END
hpp_filename = 'Constructor.hpp'
wrap_header(hpp_filename, 'Rice', nil, false, hpp_head) do |hpp|
  hpp.puts hpp_start
  for j in 0..MAX_ARGS do
    t_array = (1..j).to_a
    o_array = (j..MAX_ARGS).to_a
    typename_list    = t_array.map { |x| ", typename Arg#{x}_T" }
    type_list        = t_array.map { |x| ", Arg#{x}_T" }
    void_list        = o_array.map { |x| ", void" }

    arg_list         = t_array.map { |x| ", Arg#{x}_T arg#{x}" }
    arg_names        = t_array.map { |x| "arg#{x}" }.join(', ')

    hpp.puts fill_template(hpp_template, {
      :typename_list    => typename_list,
      :type_list        => type_list,
      :void_list        => void_list,
      :arg_list         => arg_list,
      :arg_names        => arg_names
    })
  end

  # For the self / Director system
  for j in 0..(MAX_ARGS - 1) do
    t_array = (1..j).to_a
    o_array = (j..(MAX_ARGS - 1)).to_a
    typename_list    = t_array.map { |x| ", typename Arg#{x}_T" }
    type_list        = t_array.map { |x| ", Arg#{x}_T" }
    void_list        = o_array.map { |x| ", void" }

    arg_list         = t_array.map { |x| ", Arg#{x}_T arg#{x}" }
    arg_names        = t_array.map { |x| ", arg#{x}" }

    hpp.puts fill_template(hpp_self_template, {
      :typename_list    => typename_list,
      :type_list        => type_list,
      :void_list        => void_list,
      :arg_list         => arg_list,
      :arg_names        => arg_names
    })
  end
end

if ARGV[0] == '--clean' then
  $filenames.each do |filename|
    File.rm_f(filename)
  end
end

end # if __FILE__ == $0 then


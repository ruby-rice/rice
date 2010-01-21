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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, %(typename_list)>
VALUE Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)>::
call(int argc, VALUE *argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper = (Auto_Function_Wrapper<Func_T, Ret_T, %(typenames)> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE %(scan_def_list);

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1).c_str()
        %(scan_args_list));

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 
      %(self_arg_convert_list)

      return to_ruby(wrapper->func_(%(arg_list)));
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        %(scan_args_list));

      %(arg_convert_list)

      return to_ruby(wrapper->func_(%(arg_list)));
    }
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
{
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, %(typename_list)>
VALUE Auto_Function_Wrapper<Func_T, void, %(typenames)>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Function_Wrapper<Func_T, void, %(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void, %(typenames)> *)data.get();
    Arguments* args = wrapper->arguments_;

    bool hasSelf = (self && self != Qnil);
    if(args->count() >= 0) {
      hasSelf = hasSelf && args->count() == Num_Args - 1;
    } else {
      hasSelf = hasSelf && argc == Num_Args -1;
    }

    VALUE %(scan_def_list);

    if(hasSelf) {
      rb_scan_args(argc, argv, args->formatString(Num_Args - 1) .c_str()
        %(scan_args_list));

      Arg0_T arg0 = from_ruby<Arg0_T>(self); 
      %(self_arg_convert_list)

      wrapper->func_(%(arg_list));
      return Qnil;
    } else {
      rb_scan_args(argc, argv, args->formatString(Num_Args).c_str()
        %(scan_args_list));

      %(arg_convert_list)

      wrapper->func_(%(arg_list));
      return Qnil;
    }
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
#include "traits.hpp"
END
ipp_tail = <<END
template<typename Func_T, typename Ret_T>
Auto_Function_Wrapper<Func_T, Ret_T>::
Auto_Function_Wrapper(
    Func func,
    Data_Object<Exception_Handler> handler,
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

    return to_ruby(wrapper->func_());
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
    Data_Object<Exception_Handler> handler,
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
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Function_Wrapper<Func_T, void> *)data.get();

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
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = new Arguments());

  static VALUE call();

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
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
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = new Arguments());

  static VALUE call();

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Arguments* arguments_;
};
END
ipp_filename = 'detail/Auto_Function_Wrapper.ipp'
hpp_filename = 'detail/Auto_Function_Wrapper.hpp'
wrap_header(hpp_filename, 'Rice::detail', docstring, true, hpp_head) do |hpp|
  wrap_header(ipp_filename, 'Rice::detail', nil, false, ipp_head) do |ipp|
    MAX_ARGS.downto(0) do |j|
      t_array = (0..j).to_a
      scan_def_list  = t_array.map { |x| "varg#{x}" }.join(', ')
      arg_def_list  = t_array.map { |x| "Arg#{x}_T arg#{x};"}
      arg_list      = t_array.map { |x| "arg#{x}" }.join(', ')
      scan_args_list = t_array.map { |x| ", &varg#{x}"}
      typenames     = t_array.map { |x| "Arg#{x}_T" }.join(', ')
      arg_convert_list  = t_array.map do |x|
        "typedef typename sanitize< Arg#{x}_T >::Type Arg#{x}_Type;\n\t\t" +
        "Arg#{x}_Type arg#{x} = args->getArgumentOrDefault<Arg#{x}_Type>(#{x}, varg#{x});"
      end.join("\n\t\t\t")
      self_arg_convert_list = (0...j).to_a.map do |x|
        n = x + 1
        "typedef typename sanitize< Arg#{n}_T >::Type Arg#{n}_Type;\n\t\t" +
        "Arg#{n}_Type arg#{n} = args->getArgumentOrDefault<Arg#{n}_Type>(#{x}, varg#{x});"
      end.join("\n\t\t\t")
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
        :scan_def_list    => scan_def_list,
        :arg_list         => arg_list,
        :typenames        => typenames,
        :typename_list    => typename_list,
        :arg_convert_list => arg_convert_list,
        :scan_args_list   => scan_args_list,
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
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T%(typename_list)>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T%(typenames)> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE %(scan_def_list);

    rb_scan_args(argc, argv, args->formatString(Num_Args).c_str(), 
      %(scan_args_list));

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
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T%(typename_list)>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T%(typenames)> *)data.get();
    Arguments* args = wrapper->arguments_;

    VALUE %(scan_def_list);

    rb_scan_args(argc, argv, args->formatString(Num_Args).c_str(), 
      %(scan_args_list));

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
ipp_tail = <<END
template<typename Func_T, typename Ret_T, typename Self_T>
Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T>::
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
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Ret_T, typename Self_T>
VALUE Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T> *)data.get();

    rb_scan_args(argc, argv, "0");
    Self_T * obj = from_ruby<Self_T *>(self);

    Func func = wrapper->func_;
    return to_ruby((*obj.*func)());
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

template<typename Func_T, typename Self_T>
Auto_Member_Function_Wrapper<Func_T, void, Self_T>::
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
  if(arguments == 0) {
    arguments_ = new Arguments();
  } else {
    arguments_ = arguments;
  }
}

template<typename Func_T, typename Self_T>
VALUE Auto_Member_Function_Wrapper<Func_T, void, Self_T>::
call(int argc, VALUE* argv, VALUE self)
{
  Auto_Member_Function_Wrapper<Func_T, void, Self_T> * wrapper = 0;
  try
  {
    Data_Object<Wrapped_Function> data(detail::method_data());
    wrapper =
      (Auto_Member_Function_Wrapper<Func_T, void, Self_T> *)data.get();
    rb_scan_args(argc, argv, "0");
    Self_T * obj = from_ruby<Self_T *>(self);

    Func func = wrapper->func_;
    (*obj.*func)();
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

  static VALUE call(int argc, VALUE* argv, VALUE self);

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

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

// ---------------------------------------------------------------------
END
hpp_tail = <<END
template<typename Func_T, typename Ret_T, typename Self_T>
class Auto_Member_Function_Wrapper<Func_T, Ret_T, Self_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 0;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

private:
  Func func_;
  Data_Object<Exception_Handler> handler_;
  Address_Registration_Guard handler_guard_;
  Arguments* arguments_;
};

template<typename Func_T, typename Self_T>
class Auto_Member_Function_Wrapper<Func_T, void, Self_T>
  : public Wrapped_Function
{
public:
  typedef Func_T Func;

  static const int Num_Args = 0;

  Auto_Member_Function_Wrapper(
      Func func,
      Data_Object<Exception_Handler> handler,
      Arguments* arguments = 0);

  static VALUE call(int argc, VALUE* argv, VALUE self);

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
#include "traits.hpp"
#include <typeinfo>
END
ipp_filename = 'detail/Auto_Member_Function_Wrapper.ipp'
hpp_filename = 'detail/Auto_Member_Function_Wrapper.hpp'
wrap_header(hpp_filename, 'Rice::detail', docstring, true) do |hpp|
  wrap_header(ipp_filename, 'Rice::detail', nil, false, ipp_head) do |ipp|
    MAX_ARGS.downto(0) do |j|
      t_array = (0..j).to_a
      scan_def_list  = t_array.map { |x| "varg#{x}" }.join(', ')
      arg_list      = t_array.map { |x| "arg#{x}" }.join(', ')
      scan_args_list = t_array.map { |x| "&varg#{x}"}.join(', ')
      typenames     = t_array.map { |x| ", Arg#{x}_T" }
      typenames_n   = t_array.map { |x| "Arg#{x}_T" }.join(', ')
      arg_convert_list  = t_array.map do |x|
        "typedef typename sanitize< Arg#{x}_T >::Type Arg#{x}_Type;\n\t\t" +
        "Arg#{x}_Type arg#{x} = args->getArgumentOrDefault<Arg#{x}_Type>(#{x}, varg#{x});"
      end.join("\n\t\t")
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
        :scan_def_list    => scan_def_list,
        :arg_list         => arg_list,
        :typenames        => typenames,
        :typename_list    => typename_list,
        :scan_args_list   => scan_args_list,
        :arg_convert_list => arg_convert_list,
      })
      hpp.puts fill_template(hpp_template, {
        :typenames        => typenames,
        :typename_list    => typename_list,
        :typename_list_d  => typename_list_d,
        :j                => j + 1,
        :specializations  => specializations,
      })
    end
    hpp.puts hpp_tail
    ipp.puts ipp_tail
  end
end

if ARGV[0] == '--clean' then
  $filenames.each do |filename|
    File.rm_f(filename)
  end
end

end # if __FILE__ == $0 then


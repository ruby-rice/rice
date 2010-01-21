require "rice_template"

class WrapFunctionHpp < RiceTemplate

  cpp_detail_file "wrap_function.hpp"

  def entry(num)
    {
      :typenames => build(num, "typename Arg{{i}}_T"),
      :self_typenames => ["typename Self_T", build(num - 1, "typename Arg{{i}}_T", nil)].flatten.join(", "),
      :argument_types => build(num, "Arg{{i}}_T"),
      :self_argument_types => build(num - 1, "Arg{{i}}_T")
    }
  end

end

class WrapFunctionIpp < RiceTemplate

  cpp_detail_file "wrap_function.ipp"

  def entry(num)
    {
      :typenames => build(num, "typename Arg{{i}}_T"),
      :self_typenames => ["typename Self_T", build(num - 1, "typename Arg{{i}}_T", nil)].flatten.join(", "),
      :argument_types => build(num, "Arg{{i}}_T"),
      :self_argument_types => build(num - 1, "Arg{{i}}_T"),
      :self_arg_types_with_self => ["Self_T", build(num - 1, "Arg{{i}}_T", nil)].flatten.join(", ")
    }
  end

end

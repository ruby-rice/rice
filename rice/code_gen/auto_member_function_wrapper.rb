require "rice_template"

class AutoMemberFunctionWrapperHpp < RiceTemplate

  cpp_detail_file "Auto_Member_Function_Wrapper.hpp"

  def entry(num)
    typenames = typenames_with_return = build(num, "typename Arg{{i}}_T")
    specs_with_return = "<Func_T, Ret_T, Self_T, #{build(num, "Arg{{i}}_T")}>"
    specs = "<Func_T, void, Self_T, #{build(num, "Arg{{i}}_T")}>"

    if num == self.iterations + 1
      specs_with_return = ""
      typenames_with_return = build(num, "typename Arg{{i}}_T = void")
    end

    {
      :typenames_with_return => typenames_with_return,
      :specs_with_return => specs_with_return,
      :typenames => typenames,
      :specs => specs,
      :num_args => num
    }
  end

end

class AutoMemberFunctionWrapperIpp < RiceTemplate

  cpp_detail_file "Auto_Member_Function_Wrapper.ipp"

  def entry(num)
    {
      :typenames => build(num, "typename Arg{{i}}_T"),
      :types => build(num, "Arg{{i}}_T"),
      :vargs => build(num, "varg{{i}}"),
      :scan_args => build(num, "&varg{{i}}"),
      :call_args => build(num, "arg{{i}}"),
      :arguments => arguments(num)
    }
  end

  protected

  def arguments(num)
    args = []
    num.times do |count|
      args << {
        :count => count
      }
    end

    args
  end

end

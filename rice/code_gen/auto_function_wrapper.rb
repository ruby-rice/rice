require "rice_template"

class AutoFunctionWrapperHpp < RiceTemplate

  cpp_detail_file "Auto_Function_Wrapper.hpp"

  def entry(num)
    typenames = typenames_with_return = build(num, "typename Arg{{i}}_T")
    specs_with_return = "<Func_T, Ret_T, #{build(num, "Arg{{i}}_T")}>"
    specs = "<Func_T, void, #{build(num, "Arg{{i}}_T")}>"

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

class AutoFunctionWrapperIpp < RiceTemplate

  cpp_detail_file "Auto_Function_Wrapper.ipp"

  def entry(num)
    {
      :typenames => build(num, "typename Arg{{i}}_T"),
      :types => build(num, "Arg{{i}}_T"),
      :vargs => build(num, "varg{{i}}"),
      :scan_args => build(num, "&varg{{i}}"),
      :call_args => build(num, "arg{{i}}"),
      :self_arguments => self_arguments(num),
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

  def self_arguments(num)
    args = []
    (num - 1).times do |count|
      args << {
        :in_type => "Arg#{count + 1}_T",
        :out_type => "Arg#{count+ 1}_Type",
        :arg => "arg#{count + 1}",
        :count => count
      }
    end 

    args
  end

end

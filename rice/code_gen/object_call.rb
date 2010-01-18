require "rice_template"

class ObjectCallHpp < RiceTemplate

  cpp_detail_file "object_call.hpp"

  def entry(c)
    {
      :typenames => build(c, "typename T{{i}}"),
      :arguments => build(c, "T{{i}} arg{{i}}")
    } 
  end

end

class ObjectCallIpp < RiceTemplate

  cpp_detail_file "object_call.ipp"

  def entry(c)
    {
      :typenames => build(c, "typename T{{i}}"),
      :arguments => build(c, "T{{i}} arg{{i}}"),
      :to_rubies => build(c, "to_ruby(arg{{i}})"),
      :arg_count => c
    } 
  end

end

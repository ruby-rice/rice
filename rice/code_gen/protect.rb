require "rice_template"

class ProtectIpp < RiceTemplate

  cpp_file "protect.ipp"

  def entry(c)
    {
      :count => c,
      :typenames => build(c, "typename T{{i}}"),
      :types => build(c, "T{{i}}"),
      :arguments => build(c, "T{{i}} const & t{{i}}"),
      :members => build(c, "T{{i}} const & t{{i}}_", "; "),
      :supercalls => build(c, "t{{i}}_(t{{i}})"),
      :call_args => build(c, "t{{i}}"),
      :member_call_args => build(c, "t{{i}}_")
    }
  end

end

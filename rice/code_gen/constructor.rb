require "rice_template"

class ConstructorHpp < RiceTemplate

  cpp_file "Constructor.hpp"

  def void_templates
    build(MAX_ARGS + 1, "typename Arg{{i}}_T=void")
  end

  def voids(count = MAX_ARGS)
    count < 1 ?
      "" :
      (["void"] * (count)).join(", ")
  end

  def voids_self(count = MAX_ARGS)
    voids(count - 1)
  end

  # We need to types of constructors here. One that takes
  # regular parameters and one that can take self as the first
  # parameter.
  def selfs
    [
      {:add_self => false, :with_self => "", :self_type => ""},
      {:add_self => true, :with_self => "self, ", :self_type => "Object, ", :max_args => MAX_ARGS - 1}
    ]
  end

  def entry(c)
    arg_count = MAX_ARGS - c
    arg_count -= 1 if self[:add_self]

    {
      :typenames => build(c, "typename Arg{{i}}_T"),
      :parameters => build(c, "Arg{{i}}_T arg{{i}}"),
      :arguments => build(c, "arg{{i}}"),
      :types_and_voids_list => build_types_and_voids(c, arg_count)
    }
  end

  def build_types_and_voids(c, rest)
    types = build(c, "Arg{{i}}_T", nil)
    voids = build(rest, "void", nil)

    [types, voids].flatten.join(", ")
  end

end

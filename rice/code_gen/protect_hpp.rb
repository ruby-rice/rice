require "rice_template"

class ProtectHpp < RiceTemplate

  cpp_file "protect.hpp"

  def entries
    e = []
    MAX_ARGS.times do |i|
      c = i + 1
      e << {
        :typenames => build(c, "typename T{{i}}"),
        :arguments => build(c, "T{{i}} const & t{{i}}")
      } 
    end

    e
  end

end

puts ProtectHpp.render

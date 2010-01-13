require "rice_template"

class ObjectCallHpp < RiceTemplate

  cpp_detail_file "object_call.hpp"

  def entries
    e = []
    MAX_ARGS.times do |i|
      c = i + 1
      e << {
        :typenames => build(c, "typename T{{i}}"),
        :arguments => build(c, "T{{i}} arg{{i}}")
      } 
    end

    e
  end

end

puts ObjectCallHpp.render

require 'rice_template'

class ObjectCallIpp < RiceTemplate
  cpp_detail_file "object_call.ipp"

  def entries
    e = []
    MAX_ARGS.times do |i|
      c = i + 1
      e << {
        :typenames => build(c, "typename T{{i}}"),
        :arguments => build(c, "T{{i}} arg{{i}}"),
        :to_rubies => build(c, "to_ruby(arg{{i}})"),
        :arg_count => c
      } 
    end

    e
  end

end

puts ObjectCallIpp.render

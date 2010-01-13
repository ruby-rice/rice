require "rice_template"

class ObjectCallHpp < RiceTemplate

  cpp_detail_file "object_call.hpp"

  def entries
    e = []
    MAX_ARGS.times do |i|
      c = i + 1
      e << {
        :typenames => build_typenames(c),
        :arguments => build_arguments(c)
      } 
    end

    e
  end

  protected

  def build_typenames(count)
    typenames = []
    count.times do |i|
      typenames << "typename T#{i}"
    end

    typenames.join(", ")
  end

  def build_arguments(count)
    args = []
    count.times do |i|
      args << "T#{i} arg#{i}"
    end

    args.join(", ")
  end

end

puts ObjectCallHpp.render

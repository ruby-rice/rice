require 'rice_template'

class ObjectCallIpp < RiceTemplate
  cpp_detail_file "object_call.ipp"

  def entries
    e = []
    MAX_ARGS.times do |i|
      c = i + 1
      e << {
        :typenames => build_typenames(c),
        :arguments => build_arguments(c),
        :to_rubies => build_to_rubies(c),
        :arg_count => c
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

  def build_to_rubies(count)
    rubies = []
    count.times do |i|
      rubies << "to_ruby(arg#{i})"
    end

    rubies.join(", ")
  end

end

puts ObjectCallIpp.render

require 'rubygems'
require 'mustache'

RICE_HOME = File.expand_path(File.join(File.dirname(__FILE__), ".."))

MAX_ARGS = 15

class RiceTemplate < Mustache
  # Error out on unfound tags
  self.raise_on_context_miss = true

  # Select a mustache file in rice/
  def self.cpp_file(file)
    self.template_file = File.join(RICE_HOME, "#{file}.mustache")
  end

  # Select a mustache file in rice/detail/
  def self.cpp_detail_file(file)
    self.template_file = File.join(RICE_HOME, "detail", "#{file}.mustache")
  end

  # Return the out the resulting file to be written to
  def self.out_file
    self.template_file.gsub(/\.mustache/, "")
  end

  # Render this template and print out to .out_file
  def self.render_to_file
    File.open(self.out_file, "w+") do |f|
      f.write(render)
    end
  end

  ##
  # Template helper methods
  ##

  # All templates need to implement this method to define
  # exactly what an entry contains
  def entry
    raise "Template must implement this method"
  end

  # Every template needs to loop over a set called 'entries'
  def entries
    e = []
    (self[:max_args] || MAX_ARGS).times do |i|
      e << entry(i + 1)
    end
    e
  end

  # Build _count_ number of _template_ strings and return
  # the list as a comma delimited array.
  #
  # template is expected to be a valid Mustache template.
  # This template should use {{i}} for the counter number.
  #
  # If seperator is present, returns a string. If nil, returns
  # the array.
  def build(count, template, seperator = ", ")
    args = []

    count.times do |i|
      args << Mustache.render(template, {:i => i})
    end

    if seperator
      args.join(seperator)
    else
      args
    end
  end

end

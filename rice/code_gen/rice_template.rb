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

  # Build _count_ number of _template_ strings and return
  # the list as a comma delimited array.
  #
  # template is expected to be a valid Mustache template.
  # This template should use {{i}} for the counter number.
  def build(count, template)
    args = []

    count.times do |i|
      args << Mustache.render(template, {:i => i})
    end

    args.join(", ")
  end

end

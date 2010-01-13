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

end

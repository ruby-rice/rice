require 'rubygems'
gem 'minitest'
require 'minitest/autorun'

require_relative './unittest'

TestSuite.all.each do |suite_name, test_suite|
  Object.const_set("#{suite_name}Tests", Class.new(Minitest::Test) do
    define_method("setup") do
      test_suite.run_setup
    end

    test_suite.test_cases.each do |test_case|
      define_method("test_#{test_case.name}") do
        test_case.run
#        puts test_case.run
#        assert(false)
      end
    end
  end)
end

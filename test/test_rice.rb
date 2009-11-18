require 'test/unit'
require 'rbconfig'

class RiceTest < Test::Unit::TestCase
  # TODO: probably a better way to find this out...
  VERBOSE = ARGV.include?('-v')

  EXEEXT = Config::CONFIG['EXEEXT']
  RUBY = Config::CONFIG['RUBY_INSTALL_NAME']

  def test_unittest
    run_external_test("./unittest#{EXEEXT}")
  end

  def test_vm_unittest
    run_external_test("./vm_unittest#{EXEEXT}")
  end

  def test_multiple_extensions
    run_external_test("#{RUBY} test_multiple_extensions.rb")
  end

  def test_multiple_extensions_with_inheritance
    run_external_test("#{RUBY} test_multiple_extensions_with_inheritance.rb")
  end

  def test_multiple_extensions_same_class
    run_external_test("#{RUBY} test_multiple_extensions_same_class.rb")
  end

  def run_external_test(executable)
    if VERBOSE then
      system(executable)
    else
      result = `#{executable}`
      if $? != 0 then
        puts result
      end
    end
    raise "Error: $?" if $? != 0
  end
end


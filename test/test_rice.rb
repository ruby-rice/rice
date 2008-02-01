require 'test/unit'

class RiceTest < Test::Unit::TestCase
  # TODO: probably a better way to find this out...
  VERBOSE = ARGV.include?('-v')

  def test_unittest
    run_external_test('./unittest')
  end

  def test_vm_unittest
    run_external_test('./vm_unittest')
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


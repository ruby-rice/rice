$: << File.join(File.dirname(__FILE__), 'ext')

require 'rubygems'
gem 'minitest'
require 'minitest/autorun'

class MultipleExtensionsSameClassTest < Minitest::Test
  def test_multiple_extensions_same_class
    require 't1/t1'
    require 't2/t2'

    foo = Foo.new
    assert_equal 42, foo.foo
  end
end

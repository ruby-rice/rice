$: << File.join(File.dirname(__FILE__), 'ext')

require 'test/unit'

class MultipleExtensionsSameClassTest < Test::Unit::TestCase
  def test_multiple_extensions_same_class
    require 't1/t1'
    require 't2/t2'

    foo = Foo.new
    assert_equal 42, foo.foo
  end
end

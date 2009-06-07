$: << File.join(File.dirname(__FILE__), '..', 'sample')

require 'test/unit'

class MultipleExtensionTest < Test::Unit::TestCase
  def test_multiple_extensions_with_inheritance
    # Load a library that uses inheritance
    require 'inheritance/animals'

    # Then load a different library
    require 'enum/sample_enum'

    # And make sure we can call methods in the base class on a derived
    # instance
    dog = Dog.new
    assert_equal dog.name, "Dog"
    assert_equal dog.speak, "Woof woof"
  end
end

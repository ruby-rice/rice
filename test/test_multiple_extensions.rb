$: << File.join(File.dirname(__FILE__), '..', 'sample')

require 'test/unit'

class MultipleExtensionTest < Test::Unit::TestCase
  def test_multiple_extensions
    # Rinse
    require 'map/map'
    m = Std::Map.new
    m[0] = 1

    # And repeat
    require 'enum/sample_enum'
    m = Std::Map.new
    m[0] = 1
  end
end

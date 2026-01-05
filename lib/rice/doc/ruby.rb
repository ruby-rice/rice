module Rice
  module Doc
    class Ruby
      ROOT = "https://docs.ruby-lang.org/en/master"

      def initialize
      end

      def class_url(klass)
        name = klass.name.split('::').last
        name[0].upcase! # Can't use capitalize because it lowercases other letters which breaks links like NilClass
        "#{ROOT}/#{name}.html"
      end

      def module_url(klass)
      end

      def union_url(klass)
      end

      def enum_url(klass)
      end

      def enum_value_url(klass, enum_value)
      end

      def singleton_method_url(klass, native)
      end

      def method_url(klass, native)
      end

      def attribute_url(klass, native)
      end
    end
  end
end

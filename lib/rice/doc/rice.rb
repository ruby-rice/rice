module Rice
  module Doc
    class Rice
      ROOT = "https://ruby-rice.github.io/4.x"

      CLASS_DOCS = {
        /^Rice::Buffer/ => "ruby_api/buffer",
        /^Rice::Pointer/ => "ruby_api/pointer",
        /^Rice::Reference/ => "ruby_api/reference"
      }

      def class_url(klass)
        CLASS_DOCS.each do |pattern, path|
          if klass.name.match?(pattern)
            return "#{ROOT}/#{path}"
          end
        end
        nil
      end

      def module_url(klass)
        nil
      end

      def union_url(klass)
        nil
      end

      def enum_url(klass)
        nil
      end

      def enum_value_url(klass, enum_value)
        nil
      end

      def singleton_method_url(klass, native)
        nil
      end

      def method_url(klass, native)
        class_url(klass)
      end

      def attribute_url(klass, native)
        nil
      end
    end
  end
end

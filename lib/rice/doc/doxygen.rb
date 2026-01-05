require 'open-uri'
require 'libxml-ruby'

module Rice
  module Doc
    class Doxygen
      OPERATORS = {"assign" => "operator=",
                   "assign_plus" => "operator+=",
                   "assign_minus" => "operator-=",
                   "assign_multiply" => "operator*=",
                   "assign_divide" => "operator/=",
                   "call" => "operator()",
                   "decrement" => "operator--",
                   "dereference" => "operator*",
                   "increment" => "operator++",
                   "==" => "operator==",
                   "!=" => "operator!=",
                   "+" => "operator+",
                   "-" => "operator-",
                   "*" => "operator*",
                   "/" => "operator/",
                   "&" => "operator&",
                   "|" => "operator|",
                   "<" => "operator<",
                   ">" => "operator>",
                   "<=" => "operator<=",
                   "<<" => "operator<<",
                   "[]" => "operator[]",
                   "[]=" => "operator[]"}

      def initialize(root, index, type_mappings = Hash.new, method_mappings = Hash.new)
        @root = root
        @type_mappings = type_mappings
        @method_mappings = method_mappings
        @cache = Hash.new
        @typedefs = Hash.new(Hash.new)

        data = URI.open(index).read
        parser = LibXML::XML::Parser.string(data)
        @doc = parser.parse
      end

      def camelize(content, capitalize = true)
        result = content.gsub(/(?:_)(.)/) do |matched|
                   $1.capitalize
                 end

        result[0] = capitalize ? result[0].upcase : result[0].downcase
        result
      end

      def url(member_node)
        # Build url
        anchor_file = member_node.find_first('anchorfile').content
        anchor = member_node.find_first('anchor').content
        "#{@root}/#{anchor_file}##{anchor}"
      end

      def cpp_name(klass)
        result = klass.cpp_class
        @type_mappings.each do |key, value|
          result = result.gsub(key, value)
        end
        result
      end

      def get_node(klass, xpath)
        node = @cache[klass]
        if node.nil? && !@cache.has_key?(klass)
          node = @cache[klass] = @doc.find_first(xpath)
        end
        node
      end

      def module_node(a_module)
        xpath = "//tagfile/compound[@kind='namespace'][name='#{a_module.name.downcase}']"
        result = get_node(a_module, xpath)
        if result
          read_typedefs(a_module, result)
        end
        result
      end

      def class_node(klass)
        native_name = cpp_name(klass)
        typedef_parts = native_name.split("::")
        typedef_name = typedef_parts.pop

        typedefs = @typedefs[klass.name.split("::").first]

        if typedef_type = typedefs[typedef_name]
          typedef_parts.push(typedef_type)
          native_name = typedef_parts.join("::")
        end

        xpath = "//tagfile/compound[@kind='class' or @kind='struct' or @kind='union'][name='#{native_name}']"
        get_node(klass, xpath)
      end

      def enum_node(klass)
        cpp_name = klass.cpp_class.gsub(/^enum /, '')
        cpp_parts = cpp_name.split("::")

        kind = enum_kind(klass)
        xpath = if kind == "group"
                  "//tagfile/compound[@kind='#{enum_kind(klass)}']/member[@kind='enumeration'][name='#{cpp_parts.last}']"
                else
                  parent_name = cpp_name.split("::")[0..-2].join("::")
                  "//tagfile/compound[@kind='#{enum_kind(klass)}'][name='#{parent_name}']/member[@kind='enumeration'][name='#{cpp_parts.last}']"
                end
        get_node(klass, xpath)
      end

      def read_typedefs(klass, node)
        typedefs = @typedefs[klass.name.split("::").first]
        node.find("member[@kind='typedef']").each do |typedef|
          name = typedef.find_first("name").content
          type = typedef.find_first("type").content
          type.gsub!(/(const|volatile|&|\*| )/, "")
          typedefs[name] = type
        end
      end

      def class_url(klass)
        node = class_node(klass)
        if node
          file = node.find_first('filename').content
          "#{@root}/#{file}"
        end
      end

      def module_url(klass)
      end

      def figure_parent(klass)
        # Split namespace
        names = klass.name.split('::')
        # Remove the class
        names.pop

        current_module = Object

        names.each do |name|
          current_module = current_module.const_get(name)
        end

        current_module
      end

      def enum_kind(klass)
        parent = figure_parent(klass)
        if parent == Object
          "group"
        elsif parent.is_a?(Class)
          "class"
        elsif parent.is_a?(Module)
          "namespace"
        else
          raise("Unknown parent type: #{parent}")
        end
      end

      def enum_url(klass)
        cpp_name = klass.cpp_class.gsub(/^enum /, '')
        cpp_parts = cpp_name.split("::")

        kind = enum_kind(klass)
        xpath = if kind == "group"
                  "//tagfile/compound[@kind='#{enum_kind(klass)}']/member[@kind='enumeration'][name='#{cpp_parts.last}']"
                else
                  parent_name = cpp_name.split("::")[0..-2].join("::")
                  "//tagfile/compound[@kind='#{enum_kind(klass)}'][name='#{parent_name}']/member[@kind='enumeration'][name='#{cpp_parts.last}']"
                end

        member_node = @doc.find_first(xpath)
        if member_node
          url(member_node)
        else
          nil
        end
      end

      def enum_value_url(klass, value)
        node = enum_node(klass)
        xpath = "../member[@kind='enumvalue'][name='#{value}']"
        member_node = node&.find_first(xpath)
        if member_node
          url(member_node)
        else
          nil
        end
      end

      def union_url(union)
        node = class_node(union)
        url_node = node.find_first('filename')
        "#{OPENCV_ROOT}/#{url_node.content}"
      end

      def attribute_url(klass, native)
        node = class_node(klass)

        # Find the member node
        attribute_name = camelize(native.name, false)

        xpath = "member[@kind='variable'][name='#{attribute_name}']"
        member_node = node&.find_first(xpath)
        if member_node
          url(member_node)
        else
          nil
        end
      end

      def figure_member_node(native, xpath)
        # First get all matching functions - remember C++ supports method overloads
        member_nodes = @doc.find(xpath)
        case member_nodes.size
          when 0
            return nil
          when 1
            return member_nodes.first
        end

        # Next filter members that have the same number of parameters
        filtered = member_nodes.find_all do |member_node|
          arglist_content = member_node.find_first('arglist').content
          match = arglist_content.match(/\((.*)\)/)
          tag_args = match[1].split(',')
          tag_args.size == native.parameters.size
        end

        if filtered.size == 1
          return filtered.first
        end

        # Next filter on type
        filtered.first
      end

      def singleton_method_url(klass, native)
        node = class_node(klass)
        member_name = native.name.gsub(/\?$/, "")
        member_name = camelize(member_name, false)
        xpath = "member[@kind='function' and @static='yes'][name='#{member_name}' or name='#{member_name.capitalize}']"
        member_node = node&.find_first(xpath)
        if member_node
          url(member_node)
        end
      end

      def method_url(klass, native)
        node = if klass.instance_of?(Module)
                 module_node(klass)
               else
                 class_node(klass)
               end

        ruby_name = native.name
        method_names = Array.new
        if ruby_name == "initialize" || ruby_name == "initialize_copy"
          method_names << cpp_name(klass).split('::').last
        elsif native_name = OPERATORS[ruby_name]
          method_names << native_name
        elsif native_name = @method_mappings.dig(klass.name, ruby_name)
          method_names << native_name
        elsif ruby_name.match(/\?$/)
          native_name = ruby_name.gsub(/\?$/, "")
          method_names << camelize(native_name, false)
          method_names << camelize(native_name, true)
          method_names << "is#{camelize(native_name, true)}"
        else
          method_names << ruby_name
          method_names << camelize(ruby_name, false)
          method_names << camelize(ruby_name, true)
        end

        pattern = method_names.map do |method_name|
          "name='#{method_name}'"
        end.join(" or ")

        xpath = "member[@kind='function'][#{pattern}]"
        member_node = node&.find_first(xpath)
        if member_node
          url(member_node)
        elsif native.name == "initialize"
          class_url(klass)
        #else
        #STDERR << "'#{native.name}' => '#{camelize(native.name, false)}'," << "\n"
        end
      end
    end
  end
end

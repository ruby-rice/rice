require 'erb'
require 'fileutils'

module Rice
	class Rbs
		attr_reader :extension, :output

		def initialize(extension, output)
			@extension = extension
			@output = output
		end

		def generate
			STDOUT << "Writing rbs files to #{@output}" << "\n"

			# Add the extension directory the path in case it ships with extra libraries
			ENV["PATH"] = "#{File.dirname(self.extension)}#{File::PATH_SEPARATOR}#{ENV["PATH"]}"
			require self.extension

			types = Registries.instance.types
			types.klasses.each do |klass|
				process_class(klass)
			end
		end

		def process_class(klass)
			STDOUT << "  " << klass << "\n"

			native_attributes = Registries.instance.natives.native_attributes(klass).sort.group_by(&:name)
			native_functions = Registries.instance.natives.native_functions(klass).sort.group_by(&:name)
			native_methods = Registries.instance.natives.native_methods(klass).sort.group_by(&:name)
			content = render_template("class", :klass => klass,
													      :native_attributes => native_attributes,
    													  :native_functions => native_functions,
		    											  :native_methods => native_methods)
			write_file(klass, content)
		end

		def write_file(klass, content)
			parts = klass.name.split("::")
			file_name = "#{parts.pop}.rbs"
			dir = File.join(self.output, *parts)
			FileUtils.mkdir_p(dir)

			path = File.join(dir, file_name)
			File.write(path, content, mode: "wb")
		end

		def attribute_sig(native_attributes)
			attr_type = if native_attributes.size == 2
									 "attr_accessor"
								 elsif native_attributes.first.kind == Rice::NativeKind::AttributeReader
									 "attr_reader"
								 else
									 "attr_writer"
								 end
			"#{attr_type} #{native_attributes.first.name}: #{native_attributes.first.return_klass}"
		end

		def method_sigs(native_methods, indent = 0)
			join_string = "\n" + (" " * indent) + "| "
			a = native_methods.map do |native_method|
				method_sig(native_method)
			end
			a.join(join_string)
		end

		def method_sig(native)
			params = native.parameters.map do |parameter|
				"#{parameter.arg.name}: #{parameter.klass}"
			end.join(", ")
			"(#{params}) -> #{native.return_klass}"
		end

		def render_template(template, local_variables = {})
			template = ERB.new(self.template, :trim_mode => '-')
			b = self.binding
			local_variables.each do |key, value|
				b.local_variable_set(key, value)
			end
			template.result(b)
		end

		def template
			<<~EOS
        module <%= klass.name.split("::")[0..-2].join("::") %>
          class <%= klass.name.split("::").last %>
          <%- native_functions.each do |name, functions| -%>
            def self.<%= name %>: <%= method_sigs(functions, name.length + 5) %>
          <%- end -%>
        <%= native_functions.empty? ? "" : "\n" -%>
          <%- native_attributes.each do |name, attributes| -%>
            <%= attribute_sig(attributes) %>
          <%- end -%>
        <%= native_attributes.empty? ? "" : "\n" -%>
          <%- native_methods.each do |name, methods| -%>
            def <%= name %>: <%= method_sigs(methods, name.length + 5) %>
          <%- end -%>
          end
        end
			EOS
		end
	end
end
require 'fileutils'

module Rice
	module Doc
		class Mkdocs
			ESCAPE_METHODS = {'*' => '\*',
												'+' => '\+',
												'-' => '\-',
												'.' => '\.',
												'!' => '\!',
												'|' => '\|',
												'[]' => '\[\]',
			                  '[]=' => '\[\]='}

			attr_reader :klasses, :output

			def initialize(klasses, doc_resolvers, output)
				@klasses = klasses
				@doc_resolvers = doc_resolvers
				@output = output
			end

			def generate
				FileUtils.mkdir_p(@output)

				klasses.sort_by(&:name).each.with_index do |klass, i|
					#next unless klass.name == "Std::Vector≺Cv꞉꞉Point2l≻"
					next unless klass.name.match(/^Std::Pair/)

					STDOUT << klass.name << " (" << i << "/" << klasses.count << ")" << "\n"
					if klass.instance_of?(Module)
						write_module(klass)
					elsif klass.cpp_class.match(/^enum/i)
						write_enum(klass)
					elsif klass.cpp_class.match(/^union/i)
						write_union(klass)
					else
						write_class(klass)
					end
				end
			end

			def output_path(klass)
				parts = klass.name.split("::")
				path = if klass.instance_of?(Module)
								 File.join(@output, *parts[0..-1], "index.md")
							 else
								 File.join(@output, *parts[0..-2], "#{parts.last}.md")
							 end

				unless Dir.exist?(File.dirname(path))
					FileUtils.mkdir_p((File.dirname(path)))
				end
				path
			end

			def resolver(klass)
				parts = klass.name.split("::")
				key = if parts.length == 1 && !klass.instance_of?(Module)
								nil
							else
								parts.first
							end

				@doc_resolvers[key]
			end

			def class_sig(klass)
				klass_name = klass.name.split("::").last

				url = resolver(klass)&.class_url(klass)
				if url
					"#[#{klass_name}](#{url})"
				else
					"##{klass_name}"
				end
			end

			def enum_sig(klass)
				klass_name = klass.name.split("::").last
				url = resolver(klass)&.enum_url(klass)
				if url
					"#[#{klass_name} Enum](#{url})"
				else
					"##{klass_name} Enum"
				end
			end

			def enum_value_sig(klass, value)
				url = resolver(klass)&.enum_value_url(klass, value)

				if url
					"[#{value}](#{url})"
				else
					"#{value}"
				end
			end

			def union_sig(klass)
				klass_name = klass.name.split("::").last
				url =  resolver(klass)&.union_url(klass)
				if url
					"#[#{klass_name} Union](#{url})"
				else
					"##{klass_name} Union"
				end
			end

			def method_sigs(klass, native_methods, method_sig)
				native_methods.map do |native_method|
					method_sig(klass, native_method, method_sig)
				end.join("\n\n")
			end

			def type_url(klass)
				resolver = resolver(klass)

				if klass.instance_of?(Module)
					resolver&.module_url(klass)
				elsif !klass.respond_to?(:cpp_class)
					resolver&.class_url(klass)
				elsif klass.cpp_class.match(/^enum/i)
					resolver&.enum_url(klass)
				elsif klass.cpp_class.match(/^union/i)
					resolver&.union_url(klass)
				else
					resolver&.class_url(klass)
				end
			end

			def parameters_sig(klass, native)
				native.parameters.map do |parameter|
					resolver = resolver(parameter.klass)
					url = type_url(parameter.klass)
					if url
						"#{parameter.arg.name}: [#{parameter.klass}](#{url})"
					else
						"#{parameter.arg.name}: #{parameter.klass}"
					end
				end.join(", ")
			end

			def constructor_sig(klass, native)
				url =  resolver(klass)&.method_url(klass, native)

				if url
					"[initialize](#{url})(#{parameters_sig(klass, native)})"
				else
					"initialize(#{parameters_sig(klass, native)})"
				end
			end

			def method_sig(klass, native, singleton)
				url = if singleton
								resolver(klass)&.singleton_method_url(klass, native)
							else
								resolver(klass)&.method_url(klass, native)
							end

				name = ESCAPE_METHODS[native.name] || native.name

				if url
					"[#{name}](#{url})(#{parameters_sig(klass, native)})"
				else
					"#{name}(#{parameters_sig(klass, native)})"
				end
			end

			def attribute_sig(klass, native_attributes)
				attr_type = if native_attributes.size == 2
											"read/write"
										elsif native_attributes.first.kind == NativeKind::AttributeReader
											"read"
										else
											"write"
										end

				native_attribute = native_attributes.first

				attribute_resolver = resolver(klass)
				attribute_url = attribute_resolver&.attribute_url(klass, native_attribute)

				type_resolver = resolver(native_attribute.klass)
				type_url = type_url(native_attribute.klass)

				if attribute_url
					"[#{native_attribute.name}](#{attribute_url}): [#{native_attribute.klass}](#{type_url}) (#{attr_type})"
				else
					"#{native_attribute.name}: [#{native_attribute.klass}](#{type_url}) (#{attr_type})"
				end
			end

			def write_module(a_module)
				native_singleton_functions = Registries.instance.natives.native_singleton_functions(a_module).sort.group_by(&:name)

				path = output_path(a_module)
				File.open(path, 'w') do |file|
					file << "#" << a_module.name << "\n"
					unless native_singleton_functions.empty?
						file << "## Singleton Methods" << "\n"
						native_singleton_functions.each do |name, natives|
							file << method_sigs(a_module, natives, false) << "\n\n"
						end
						file << "\n"
					end
				end
			end

			def write_class(klass)
				native_attributes = Registries.instance.natives.native_attributes(klass).sort.group_by(&:name)
				native_singleton_functions = Registries.instance.natives.native_singleton_functions(klass).sort.group_by(&:name)
				native_methods = Registries.instance.natives.native_methods(klass).sort.group_by(&:name)

				path = output_path(klass)
				File.open(path, 'w') do |file|
					file << class_sig(klass) << "\n"

					# Singleton methods
					unless native_singleton_functions.empty?
						file << "## Singleton Methods" << "\n"
						native_singleton_functions.each do |name, natives|
							file << method_sigs(klass, natives, true) << "\n\n"
						end
						file << "\n"
					end

					# Attributes
					unless native_attributes.empty?
						file << "## Attributes" << "\n"
						native_attributes.each do |name, natives|
							file << attribute_sig(klass, natives) << "\n\n"
						end
						file << "\n"
					end

					# Constructor
					constructors = native_methods["initialize"]
					if constructors
						file << "## Constructors" << "\n"
						constructors.each do |constructor|
							file << constructor_sig(klass, constructor) << "\n\n"
						end
						file << "\n"
					end

					unless native_methods.empty?
						file << "## Methods" << "\n"
						native_methods.each do |name, natives|
							next if name == "initialize"
							file << method_sigs(klass, natives, false) << "\n\n"
						end
						file << "\n"
					end
				end
			end

			def write_enum(klass)
				klass_name = klass.name.split('::').last

				path = output_path(klass)
				File.open(path, 'w') do |file|
					file << enum_sig(klass) << "\n"
					file << "## Values" << "\n"
					klass.values.each do |value|
						file << "* " << enum_value_sig(klass, value) << "\n"
					end
				end
			end

			def write_union(klass)
				klass_name = klass.name.split('::').last
				native_attributes = Registries.instance.natives.native_attributes(klass).sort.group_by(&:name)

				path = output_path(klass)
				File.open(path, 'w') do |file|
					file << union_sig(klass) << "\n"

					# Attributes
					unless native_attributes.empty?
						file << "## Attributes" << "\n"
						native_attributes.each do |name, natives|
							file << attribute_sig(klass, natives) << "\n\n"
						end
						file << "\n"
					end
				end
			end
		end
	end
end

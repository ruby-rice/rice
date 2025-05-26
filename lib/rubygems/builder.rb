class Gem::Ext::Builder
	alias :builder_for_original :builder_for
	def builder_for(extension)
		case extension
			when /CMakeLists.txt/ then
				Gem::Ext::CmakeBuilder.new
			else
				builder_for_original(extension)
		end
	end
end

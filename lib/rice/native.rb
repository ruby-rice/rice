module Rice
	class Native
		include Comparable

		def <=>(other)
			case
				when self.name == other.name
					self.parameters.size <=> other.parameters.size
				when self.name == "initialize"
					-1
				when self.name == "initialize"
					1
				else
					self.name <=> other.name
			end
		end
	end
end
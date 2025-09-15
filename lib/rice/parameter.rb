module Rice
	class Parameter
		def to_s
			"Parameter<#{self.arg.name}: #{self.klass.name}>"
		end
	end
end
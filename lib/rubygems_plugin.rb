# For older versions of RubyGems replace the default CMakeBuilder with a
# more modern one
if Gem.rubygems_version < Gem::Version.new('4.0')
  Gem.pre_install do |installer|
    extensions = installer.package&.spec&.extensions
    if extensions && extensions.grep(/CMakeLists/)
      require_relative 'rubygems/builder'
      require_relative 'rubygems/cmake_builder'
    end
    true
  end
end

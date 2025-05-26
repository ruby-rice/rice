# Create RubyGems hook so we can replace the default CMakeBuilder with our own version
Gem.pre_install do |installer|
  extensions = installer.package.spec.extensions
  if extensions.grep(/CMakeLists/)
    require_relative 'rubygems/builder'
    require_relative 'rubygems/cmake_builder'
  end
end
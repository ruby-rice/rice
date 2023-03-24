# CMakeList.txt : CMake project for cmake, include source and define
# project specific logic here.
#

include_directories ("../..")

# Add source to this project's executable.
add_library (sample_enum SHARED
			 "sample_enum.cpp")

if (MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  target_compile_options(sample_enum PRIVATE /bigobj /std:c++17)
  include_directories ("C:/msys64/usr/local/ruby-3.2.1-vc/include/ruby-3.2.0")
  include_directories ("C:/msys64/usr/local/ruby-3.2.1-vc/include/ruby-3.2.0/x64-mswin64_140")
  target_link_libraries(sample_enum "C:/msys64/usr/local/ruby-3.2.1-vc/lib/x64-vcruntime140-ruby320.lib")
endif ()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET unittest PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

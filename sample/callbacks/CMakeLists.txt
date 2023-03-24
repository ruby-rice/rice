# CMakeList.txt : CMake project for cmake, include source and define
# project specific logic here.
#

include_directories ("../..")

# Add source to this project's executable.
add_library (sample_callbacks SHARED
			 "sample_callbacks.cpp")

if (MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  target_compile_options(sample_callbacks PRIVATE /bigobj)
  include_directories ("C:/msys64/usr/local/ruby-3.2.1-vc/include/ruby-3.2.0")
  include_directories ("C:/msys64/usr/local/ruby-3.2.1-vc/include/ruby-3.2.0/x64-mswin64_140")
  target_link_libraries(sample_callbacks "C:/msys64/usr/local/ruby-3.2.1-vc/lib/x64-vcruntime140-ruby320.lib")
endif ()

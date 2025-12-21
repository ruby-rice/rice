add_library(Rice INTERFACE)
add_library(Rice::Rice ALIAS Rice)

target_include_directories(rice INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Need C++17 or higher
target_compile_features(rice INTERFACE cxx_std_17)

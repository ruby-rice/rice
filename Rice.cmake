add_library(Rice INTERFACE)
add_library(Rice::Rice ALIAS Rice)

target_include_directories(Rice INTERFACE
  $<BUILD_INTERFACE:$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}>>
  $<BUILD_INTERFACE:$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/include>>
  $<INSTALL_INTERFACE:include>
)

# Need C++17 or higher
target_compile_features(Rice INTERFACE cxx_std_17)

include_guard(GLOBAL)

if(NOT TARGET Rice)
  add_library(Rice INTERFACE)

  target_include_directories(Rice INTERFACE
    $<BUILD_INTERFACE:$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}>>
    $<BUILD_INTERFACE:$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/include>>
    $<INSTALL_INTERFACE:include>
  )

  # Need C++17 or higher
  target_compile_features(Rice INTERFACE cxx_std_17)
endif()

if(NOT TARGET Rice::Rice)
  add_library(Rice::Rice ALIAS Rice)
endif()

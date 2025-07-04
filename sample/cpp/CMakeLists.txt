project(BitMapPlusPlus)

# Add source to this project's executable.
add_library (BitMapPlusPlus SHARED
             "BitMapPlusPlus.def"
             "BitmapPlusPlus-rb.cpp")

# Rice headers
target_include_directories(BitMapPlusPlus PRIVATE ../..)

# Ruby headers and libraries
target_include_directories(BitMapPlusPlus PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(BitMapPlusPlus PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(BitMapPlusPlus ${Ruby_LIBRARY})

set_target_properties(BitMapPlusPlus PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(BitMapPlusPlus PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

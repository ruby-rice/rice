project(BitmapPlusPlus)

# Add source to this project's executable.
add_library (BitmapPlusPlus SHARED
             "BitmapPlusPlus.def"
             "BitmapPlusPlus-rb.cpp")

# Rice headers
target_include_directories(BitmapPlusPlus PRIVATE ../..)

# Ruby headers and libraries
target_include_directories(BitmapPlusPlus PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(BitmapPlusPlus PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(BitmapPlusPlus ${Ruby_LIBRARY})

# Output directory
set_target_properties(BitmapPlusPlus PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(BitmapPlusPlus PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

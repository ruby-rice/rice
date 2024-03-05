# Add source to this project's executable.
add_library (map SHARED
			 "map.cpp")

target_include_directories(map PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(map PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(map ${Ruby_LIBRARY})
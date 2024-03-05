# Add source to this project's executable.
add_library (t2 SHARED
			 "t2.cpp")

target_include_directories(t2 PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(t2 PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(t2 ${Ruby_LIBRARY})
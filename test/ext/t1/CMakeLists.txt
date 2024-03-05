# Add source to this project's executable.
add_library (t1 SHARED
			 "t1.cpp")

target_include_directories(t1 PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(t1 PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(t1 ${Ruby_LIBRARY})
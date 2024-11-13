# Add source to this project's executable.
add_library (animals SHARED
			 "animals.cpp")

target_include_directories(animals PRIVATE ../../include)
target_include_directories(animals PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(animals PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(animals ${Ruby_LIBRARY})
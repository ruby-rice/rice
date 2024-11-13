# Add source to this project's executable.
add_library (sample_enum SHARED
			 "sample_enum.cpp")

target_include_directories(sample_enum PRIVATE ../../include)
target_include_directories(sample_enum PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(sample_enum PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(sample_enum ${Ruby_LIBRARY})
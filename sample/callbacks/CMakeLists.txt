# Add source to this project's executable.
add_library (sample_callbacks SHARED
			 "sample_callbacks.cpp")

target_include_directories(sample_callbacks PRIVATE ../../include)
target_include_directories(sample_callbacks PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(sample_callbacks PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(sample_callbacks ${Ruby_LIBRARY})
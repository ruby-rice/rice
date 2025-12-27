cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MESSAGE_LOG_LEVEL DEBUG)

project(unittest LANGUAGES CXX)

# ----- Sources ------
# Add source to this project's executable.
add_executable(${PROJECT_NAME}
               "unittest.cpp"
               "embed_ruby.cpp"
               "test_Overloads.cpp"
)

# ----- Dependencies ------
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Ruby
#find_package(Ruby Required)
include("../FindRuby.cmake")
target_link_libraries(${PROJECT_NAME} PRIVATE Ruby::Ruby)

# FFI
if (MSVC) # This will include Clang on Windows
    find_package(unofficial-libffi CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::libffi::libffi)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_LIBFFI)
elseif (APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFI REQUIRED IMPORTED_TARGET libffi)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE PkgConfig::FFI)
else()
    find_library(FFI_LIBRARY NAMES ffi REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${FFI_LIBRARY})
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_LIBFFI)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_LIBFFI)

# Rice
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

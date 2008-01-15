project(Rice CXX)

# Ruby checking here, because the FindRuby script that comes with some CMake builds is broken:

if(RUBY_LIBRARY AND RUBY_INCLUDE_PATH)
   # Already in cache, be silent
   set(RUBY_FIND_QUIETLY TRUE)
endif (RUBY_LIBRARY AND RUBY_INCLUDE_PATH)

FIND_PROGRAM(RUBY_EXECUTABLE NAMES ruby ruby1.8 ruby18 )

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['archdir']"
   OUTPUT_VARIABLE RUBY_ARCH_DIR)

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['libdir']"
   OUTPUT_VARIABLE RUBY_LIB_DIR)

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['LIBRUBY']"
   OUTPUT_VARIABLE RUBY_LIBRARY)

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['LIBRUBY']"
   OUTPUT_VARIABLE RUBY_LIBRARY)

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['LDFLAGS']"
   OUTPUT_VARIABLE RUBY_LIB_FLAGS)

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['LIBS']"
   OUTPUT_VARIABLE RUBY_LIBS)

FIND_PATH(RUBY_INCLUDE_PATH
   NAMES ruby.h
  PATHS
   ${RUBY_ARCH_DIR}
  /usr/lib/ruby/1.8/i586-linux-gnu/ )

# End Find Ruby

add_subdirectory(rice)
add_subdirectory(test)

# find_package(ruby)

include_directories(${RUBY_INCLUDE_PATH})
include_directories(.)

link_directories(${RUBY_LIB_DIR})
link_directories(${RUBY_ARCH_DIR})
link_directories(${RUBY_LIB_FLAGS})
link_directories(rice)

file(GLOB_RECURSE SOURCES rice/*cpp)
file(GLOB_RECURSE TESTS test/*cpp)

# But we don't want test_VM in the tests list
list(REMOVE_AT TESTS -2)

add_library(rice STATIC ${SOURCES})
add_executable(unittest ${TESTS})
add_executable(vm_unittest test/test_VM.cpp test/unittest.cpp)

target_link_libraries(unittest ${RUBY_LIBRARY})
target_link_libraries(unittest rice)

target_link_libraries(vm_unittest ${RUBY_LIBRARY})
target_link_libraries(vm_unittest rice)

# Now take care of the samples
add_library(sample_enum SHARED sample/enum/sample_enum.cpp)
target_link_libraries(sample_enum ${RUBY_LIBRARY})
target_link_libraries(sample_enum rice)

#add_executable(sample_enum sample/enum/sample_enum.cpp)
#target_link_libraries(sample_enum rice)

#add_executable(sample_enum sample/enum/sample_enum.cpp)
#target_link_libraries(sample_enum rice)

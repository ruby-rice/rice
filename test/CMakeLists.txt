# Include sub-directories.
add_subdirectory ("ext/t1")
add_subdirectory ("ext/t2")

# Add source to this project's executable.
add_executable (unittest
			"unittest.cpp"
			"embed_ruby.cpp"
			"test_Address_Registration_Guard.cpp"
			"test_Array.cpp"
			"test_Attribute.cpp"
			"test_Builtin_Object.cpp"
			"test_Class.cpp"
			"test_Constructor.cpp"
			"test_Data_Object.cpp"
			"test_Data_Type.cpp"
			"test_Director.cpp"
			"test_Enum.cpp"
			"test_Exception.cpp"
			"test_Hash.cpp"
			"test_global_functions.cpp"
			"test_Identifier.cpp"
			"test_Inheritance.cpp"
			"test_Iterator.cpp"
			"test_Jump_Tag.cpp"
			"test_Keep_Alive.cpp"
			"test_Memory_Management.cpp"
			"test_Module.cpp"
			"test_Object.cpp"
			"test_Ownership.cpp"
			"test_Self.cpp"
			"test_Stl_Map.cpp"
			"test_Stl_Optional.cpp"
			"test_Stl_Pair.cpp"
			"test_Stl_Reference_Wrapper.cpp"
			"test_Stl_SmartPointer.cpp"
			"test_Stl_String.cpp"
			"test_Stl_String_View.cpp"
			"test_Stl_Unordered_Map.cpp"
			"test_Stl_Variant.cpp"
			"test_Stl_Vector.cpp"
			"test_String.cpp"
			"test_Struct.cpp"
			"test_Symbol.cpp"
			"test_To_From_Ruby.cpp"
			"test_Tracking.cpp")

target_include_directories(unittest PRIVATE ${Ruby_INCLUDE_DIR} ${Ruby_CONFIG_INCLUDE_DIR})
target_include_directories(unittest PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(unittest ${Ruby_LIBRARY})
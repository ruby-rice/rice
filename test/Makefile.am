noinst_PROGRAMS = unittest vm_unittest

SUBDIRS = ext

TESTS = unittest vm_unittest


check: run_multiple_extensions_test

.PHONY: run_multiple_extensions_test

run_multiple_extensions_test:
	$(RUBY) test_multiple_extensions.rb


check: run_multiple_extensions_with_inheritance_test

.PHONY: run_multiple_extensions_with_inheritance_test

run_multiple_extensions_with_inheritance_test:
	$(RUBY) test_multiple_extensions_with_inheritance.rb


check: run_multiple_extensions_same_class_test

.PHONY: run_multiple_extensions_same_class_test

run_multiple_extensions_same_class_test:
	$(RUBY) test_multiple_extensions_same_class.rb


unittest_SOURCES = \
	unittest.cpp \
	test_Address_Registration_Guard.cpp \
	test_Allocation_Strategies.cpp \
	test_Array.cpp \
	test_Builtin_Object.cpp \
	test_Class.cpp \
	test_Constructor.cpp \
	test_Critical_Guard.cpp \
	test_Data_Object.cpp \
	test_Data_Type.cpp \
	test_Director.cpp \
	test_Enum.cpp \
	test_Exception.cpp \
	test_Hash.cpp \
	test_Identifier.cpp \
	test_Jump_Tag.cpp \
	test_Memory_Management.cpp \
	test_Module.cpp \
	test_Object.cpp \
	test_String.cpp \
	test_Struct.cpp \
	test_Symbol.cpp \
	test_To_From_Ruby.cpp \
	test_global_functions.cpp

vm_unittest_SOURCES = \
	unittest.cpp \
	test_VM.cpp

AM_CPPFLAGS = \
	-I.. \
	$(RUBY_CFLAGS)
	$(RUBY_CPPFLAGS)

AM_CXXLAGS = \
	$(RUBY_CXXFLAGS)

AM_LDFLAGS = \
	$(RUBY_LDFLAGS) \
	-L../rice

LIBS = \
	-lrice \
	$(RUBY_LIBS) \
	$(RUBY_LIBRUBYARG)


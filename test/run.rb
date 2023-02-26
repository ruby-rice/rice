require 'ffi'
module Unittest
 extend FFI::Library
end
Unittest.ffi_lib './unittest.so'
Unittest.attach_function :main, [:int, :pointer], :int

argv = FFI::MemoryPointer.new(:pointer, 1)
argv[0].put_pointer(0, FFI::MemoryPointer.from_string('unittest'))

Unittest.main 1, argv


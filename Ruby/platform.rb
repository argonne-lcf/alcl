require 'opencl_ruby_ffi'
require 'narray_ffi'

OpenCL::platforms.each { |p| puts p.name }

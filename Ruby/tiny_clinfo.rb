require 'opencl_ruby_ffi'
require 'narray_ffi'

OpenCL::platforms.each do |p|
  puts p.name
  p.devices.each { |d| puts "  --Device: #{d.name}" }
end

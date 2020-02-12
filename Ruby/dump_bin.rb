require 'opencl_ruby_ffi'
require 'narray_ffi'

if ARGV.length < 4
  raise "#{$0} platform_id device_id input output"
end

platform_index   = ARGV[0].to_i
device_index     = ARGV[1].to_i
global_work_size = ARGV[2].to_i
local_work_size  = ARGV[3].to_i


plat = OpenCL.platforms[platform_index]
puts "Platform: #{plat.name}"
dev = plat.devices[device_index]
puts "  -- Device: #{dev.name}"

context = OpenCL.create_context([dev])

source = File::read("hwv.cl")

program = context.create_program_with_source(source)
program.build

File::open("hwv.bin", "wb") { |f|
  f.write program.binaries[0][1]
}

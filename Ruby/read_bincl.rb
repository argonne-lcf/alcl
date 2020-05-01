require 'opencl_ruby_ffi'
require 'narray_ffi'

if ARGV.length < 4
  raise "#{$0} platform_id device_id global_size local_size"
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
queue = context.create_command_queue(dev)

binary = File::read("hwv.bin", mode: "rb")
program, status = context.create_program_with_binary([dev], binary )
program.build

kernel = program.create_kernel(:hello_world)
queue.enqueue_ndrange_kernel(kernel, [global_work_size], local_work_size: [local_work_size])
queue.finish

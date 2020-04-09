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

source = <<EOF
__kernel void hello_world(__global float *a, const unsigned int n) {
  const int world_rank = get_global_id(0);
  if (world_rank < n)
    a[world_rank] =  world_rank;
}
EOF

#host memory
h_a = NArray.sfloat(global_work_size)
#device buffer
d_a = context.create_buffer(global_work_size*h_a.element_size, flags: [OpenCL::Mem::WRITE_ONLY, OpenCL::Mem::HOST_READ_ONLY])

program = context.create_program_with_source(source)
begin
  program.build
rescue
  puts program.build_log
  exit
end
kernel = program.create_kernel(:hello_world)
kernel.set_args(d_a, OpenCL::Int1::new(global_work_size))
queue.enqueue_ndrange_kernel(kernel, [global_work_size], local_work_size: [local_work_size])
queue.enqueue_read_buffer(d_a, h_a)
queue.finish

sum = h_a.sum

puts "final result: #{sum}, should have been #{global_work_size*(global_work_size-1)/2}"

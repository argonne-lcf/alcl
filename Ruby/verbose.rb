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
__kernel void hello_world() {
  const int world_rank = get_global_id(0);
  const int work_size =  get_global_size(0);
  const int local_rank = get_local_id(0);
  const int local_size = get_local_size(0);
  const int group_rank = get_group_id(0);
  const int group_size = get_num_groups(0);
  printf("Hello world: World rank/size: %d / %d. Local rank/size: %d / %d  Group rank/size: %d / %d \\n", world_rank, work_size, local_rank, local_size, group_rank, group_size);
}
EOF

program = context.create_program_with_source(source)
program.build
kernel = program.create_kernel(:hello_world)
queue.enqueue_ndrange_kernel(kernel, [global_work_size], local_work_size: [local_work_size])
queue.finish

require 'opencl_ruby_ffi'
require 'narray_ffi'

if ARGV.length < 5
  raise "#{$0} platform_id device_id global_size local_size num_kernel"
end

platform_index   = ARGV[0].to_i
device_index     = ARGV[1].to_i
global_work_size = ARGV[2].to_i
local_work_size  = ARGV[3].to_i
num_kernel       = ARGV[4].to_i


plat = OpenCL.platforms[platform_index]
puts "Platform: #{plat.name}"
dev = plat.devices[device_index]
puts "  -- Device: #{dev.name}"

context = OpenCL.create_context([dev])
queue = context.create_command_queue(dev, :properties => OpenCL::CommandQueue::OUT_OF_ORDER_EXEC_MODE_ENABLE)

source = <<EOF
__kernel void hello_world(const int id) {
  printf(\"Hello world from id %d \\n", id);
}
EOF

program = context.create_program_with_source(source)
program.build
kernel = program.create_kernel(:hello_world)

event = OpenCL::create_user_event(context)

num_kernel.times { |id|
  kernel.set_args(OpenCL::Int1::new(id))
  if id % 2 == 0
    queue.enqueue_ndrange_kernel(kernel, [global_work_size], local_work_size: [local_work_size])
  else
    queue.enqueue_ndrange_kernel(kernel, [global_work_size], local_work_size: [local_work_size], :event_wait_list => [event])
  end
}

sleep(1)
event.set_status(OpenCL::CommandExecutionStatus::COMPLETE)
queue.finish

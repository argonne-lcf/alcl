import sys
import numpy as np
import pyopencl as cl

if len(sys.argv) < 6:
	raise ValueError(sys.argv[0] + " platform_id device_id global_size local_size num_kernel")

platform_index   = int(sys.argv[1])
device_index     = int(sys.argv[2])
global_work_size = int(sys.argv[3])
local_work_size  = int(sys.argv[4])
num_kernel       = int(sys.argv[5])

plat = cl.get_platforms()[platform_index]
print("Platform: " + plat.name)
dev = plat.get_devices()[device_index]
print("  -- Device: " + dev.name)

context = cl.Context(devices=[dev])
queue = cl.CommandQueue(context, dev, properties=cl.command_queue_properties.OUT_OF_ORDER_EXEC_MODE_ENABLE |
                                                 cl.command_queue_properties.PROFILING_ENABLE)

source = '''
__kernel void hello_world(const int id) {
  printf(\"Hello world from id %d \\n", id);
}
'''

program = cl.Program(context, source)
program.build()
kernel = cl.Kernel(program, "hello_world");

events = []

for i in range(num_kernel):
	kernel.set_args(np.int32(i))
	if i == 0:
		event = cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size])
	else:
		event = cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size], wait_for=[events[-1]])
	events.append(event)

cl.wait_for_events(events)

start = events[0].profile.start
end = events[-1].profile.complete

print("OpenCl Execution time is:", (end-start) * 1e-6 , "milliseconds")

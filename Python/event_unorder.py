import sys
import time
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
queue = cl.CommandQueue(context, dev, properties=cl.command_queue_properties.OUT_OF_ORDER_EXEC_MODE_ENABLE)

source = '''
__kernel void hello_world(const int id) {
  printf(\"Hello world from id %d \\n", id);
}
'''

program = cl.Program(context, source)
program.build()
kernel = cl.Kernel(program, "hello_world");

event = cl.UserEvent(context)

for i in range(num_kernel):
	kernel.set_args(np.int32(i))
	if i % 2 == 0:
		cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size])
	else:
		cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size], wait_for=[event])

time.sleep(1)
event.set_status(cl.command_execution_status.COMPLETE)
queue.finish()

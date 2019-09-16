import sys
import numpy as np
import pyopencl as cl

if len(sys.argv) < 5:
	raise ValueError(sys.argv[0] + " platform_id device_id global_size local_size")

platform_index   = int(sys.argv[1])
device_index     = int(sys.argv[2])
global_work_size = int(sys.argv[3])
local_work_size  = int(sys.argv[4])

plat = cl.get_platforms()[platform_index]
print("Platform: " + plat.name)
dev = plat.get_devices()[device_index]
print("  -- Device: " + dev.name)

context = cl.Context(devices=[dev])
queue = cl.CommandQueue(context, dev)

source = '''
__kernel void hello_world(__global double *a, const unsigned int n) {
  const int world_rank = get_global_id(0);
  if (world_rank < n)
    a[world_rank] =  world_rank;
}
'''

program = cl.Program(context, source)
program.build()
kernel = cl.Kernel(program, "hello_world");

#host memory
h_a = np.empty(global_work_size)
#device buffer
d_a = cl.Buffer(context, cl.mem_flags.WRITE_ONLY | cl.mem_flags.HOST_READ_ONLY, size=h_a.nbytes)

kernel.set_args(d_a, np.int32(global_work_size))
cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size])
cl.enqueue_copy(queue, h_a, d_a)
queue.finish()

sum = np.add.reduce(h_a)
print("final result:", sum, ", should have been", global_work_size*(global_work_size-1)/2)

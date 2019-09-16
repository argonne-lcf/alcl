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
__kernel void hello_world() {
  const int world_rank = get_global_id(0);
  const int work_size =  get_global_size(0);
  const int local_rank = get_local_id(0);
  const int local_size = get_local_size(0);
  const int group_rank = get_group_id(0);
  const int group_size = get_num_groups(0);
  printf("Hello world: World rank/size: %d / %d. Local rank/size: %d / %d  Group rank/size: %d / %d \\n", world_rank, work_size, local_rank, local_size, group_rank, group_size);
}
'''

program = cl.Program(context, source)
program.build()
kernel = cl.Kernel(program, "hello_world");
cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size])
queue.finish()

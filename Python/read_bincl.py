import sys
import os
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

binary = open("hwv.bin", "rb").read()

program = cl.Program(context, [dev], [binary])
program.build()
kernel = cl.Kernel(program, "hello_world");
cl.enqueue_nd_range_kernel(queue, kernel, [global_work_size], [local_work_size])
queue.finish()

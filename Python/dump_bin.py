import sys
import numpy as np
import pyopencl as cl

if len(sys.argv) < 5:
	raise ValueError(sys.argv[0] + " platform_id device_id")

platform_index   = int(sys.argv[1])
device_index     = int(sys.argv[2])

plat = cl.get_platforms()[platform_index]
print("Platform: " + plat.name)
dev = plat.get_devices()[device_index]
print("  -- Device: " + dev.name)

context = cl.Context(devices=[dev])

source = open("hwv.cl","r").read()

program = cl.Program(context, source)
program.build()
open("hwv.bin","wb").write(program.binaries[0])

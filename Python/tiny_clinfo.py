import numpy as np
import pyopencl as cl

platforms = cl.get_platforms()
for plat in platforms:
	print("Platform: " + plat.name)
	devices = plat.get_devices()
	for dev in devices:
		print("  -- Device: " + dev.name)

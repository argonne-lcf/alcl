import numpy as np
import pyopencl as cl

platforms = cl.get_platforms()
for plat in platforms:
	print(plat.name)

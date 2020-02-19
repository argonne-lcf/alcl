```
  ___   _     _____  _       _____     _             _       _ 
 / _ \ | |   /  __ \| |     |_   _|   | |           (_)     | |
/ /_\ \| |   | /  \/| |       | |_   _| |_ ___  _ __ _  __ _| |
|  _  || |   | |    | |       | | | | | __/ _ \| '__| |/ _` | |
| | | || |___| \__/\| |____   | | |_| | || (_) | |  | | (_| | |
\_| |_/\_____/\____/\_____/   \_/\__,_|\__\___/|_|  |_|\__,_|_|
                                                               
                                                               
```

Argonne Leadership Computing Facility OpenCL tutorial.

Examples are provided in C, C++, Python and Ruby.

```bash
# Create all the executable
make
# Run all the executable
make run
```

For Python examples you need to have pyopencl installed.
For Ruby you need the opencl_ruby_ffi gem installed.

# Introduction to the API

## platform (Python: query_platform.py)

- Simple OpenCL C example to list your platform
- `make run_platform`

## tiny_clinfo

- List all the device for all the platform in your system (inspired by the "standard" `clinfo` tools)
- `make run_tiny_clinfo`

# Hello world and ID

## my_first_kernel

- Hello world kernel. Each work-item will print is `global_id` (`rank` in MPI terminology)
- `make run_my_first_kernel`
- `Usage: ./my_first_kernel platform_id device_id global_size local_size`

## verbose

- Verbose kernel, more introspective. Each work-item will print is `global` and `local id`. And the associated total size.
- `make run_verbose`
- `Usage: ./my_first_kernel platform_id device_id global_size local_size`

# Where do I put my kernel?

## read_cl

- Similar to verbose, but we read the kernel string from a file
- `make run_read_cl`
- `Usage: ./read_cl platform_id device_id global_size local_size`


## read_bincl

- Similar to verbose, but we precopile the kernel and then read it
- `make run_read_bincl`
- `Usage: ./read_bincl platform_id device_id global_size local_size`


## read_elfcl (C and C++ only)

- Similar to verbose, but store the precompiler opencl kernel inside the execetuable.
- `make run_read_elfcf`
- `Usage: ./read_elfcl platform_id device_id global_size local_size`

# Buffer

## buffer (Python: buffer_usage.py)
- How to create and use buffer
- `make run_buffer `

# Ordering

## event_unorder

- How to use and out of order queue
- `make run_event_unorder`
- `Usage: ./event_unorder platform_id device_id global_size local_size number_of_kernel_launch`

##  event_order

- How to use and out of order queue and event to impose and order
- `make run_event_order`
- `Usage: ./event_unorder platform_id device_id global_size local_size number_of_kernel_launch`

# Profiling

## profiling
- How to measure exution time of a kernel
- `make run_profiling`

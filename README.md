```
  ___   _     _____  _       _____     _             _       _ 
 / _ \ | |   /  __ \| |     |_   _|   | |           (_)     | |
/ /_\ \| |   | /  \/| |       | |_   _| |_ ___  _ __ _  __ _| |
|  _  || |   | |    | |       | | | | | __/ _ \| '__| |/ _` | |
| | | || |___| \__/\| |____   | | |_| | || (_) | |  | | (_| | |
\_| |_/\_____/\____/\_____/   \_/\__,_|\__\___/|_|  |_|\__,_|_|
                                                               
                                                               
```

Argonne Leadership Computing Facility OpenCL tutorial

```
# Create all the executable
make
# Run all the executable
make run
```

## platform{,.cpp}

- Simple OpenCL C example to list your platform
- `make run_platform`

## tiny_clinfo{,.cpp}

- List all the device for all the platform in your system (inspired by the "standard" `clinfo` tools)
- `make run_tiny_clinfo`

## my_first_kernel{,.cpp}

- Hello world kernel. Each work-item will print is `global_id` (`rank` in MPI terminology)
- `make run_my_first_kernel`
- `Usage: ./my_first_kernel platform_id device_id global_size local_size`

## verbose{,.cpp}

- Verbose kernel, more introspective. Each work-item will print is `global` and `local id`. And the associated total size.
- `make run_verbose`
- `Usage: ./my_first_kernel platform_id device_id global_size local_size`


## read_cl{,.cpp}

- Similar to verbose, but we read the kernel string from a file
- `make run_read_cl`
- `Usage: ./read_cl platform_id device_id global_size local_size`


## read_bincl{,.cpp}
### Warning. Required `ioc64` (The intel OpenCL offline compiler)

- Similar to verbose, but we precopile the kercel and then read it
- `make run_read_bincl`
- `Usage: ./read_bincl platform_id device_id global_size local_size`


## read_elfcl{,.cpp}
### Warning. Required `ioc64` (The intel OpenCL offline compiler)

- Similar to verbose, but store the precompiler opencl kernel inside the execetuable.
- `make run_read_elfcf`
- `Usage: ./read_elfcl platform_id device_id global_size local_size`

# Buffer
- How to create and use buffer
- `make run_buffer `

# Ordering

##  event_unorder{,.cpp}

- How to use and out of order queue
- `make run_event_unorder`
- `Usage: ./event_unorder platform_id device_id global_size local_size number_of_kernel_launch`

##  event_order{,.cpp}

- How to use and out of order queue and event to impose and order
- `make run_event_order`
- `Usage: ./event_unorder platform_id device_id global_size local_size number_of_kernel_launch`

# Profiling
- How to measure exution time of a kernel
- `make run_profiling`





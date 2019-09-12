# alcl
Argonne Leadership Computing Facility OpenCL tutorial

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

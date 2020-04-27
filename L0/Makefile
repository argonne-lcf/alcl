CC=gcc
CFLAGS=-g -O2 -lze_loader -Wall
BINARIES=tiny_zeinfo my_first_kernel profiling kernel_multithreaded
all: $(BINARIES)

run: $(addprefix run_, $(BINARIES))

clean:
	rm -f -- $(BINARIES) metric
	rm -f -- hwv.ar
	
tiny_zeinfo: tiny_zeinfo.c ze_utils.c
	    $(CC) $^ $(CFLAGS) -o $@

run_tiny_zeinfo: tiny_zeinfo
	    ./$<

hwv.ar: hwv.cl
	ocloc compile -file $^ -device \*

my_first_kernel: my_first_kernel.c ze_utils.c | hwv.ar
	        $(CC) $^ $(CFLAGS) -o $@

run_my_first_kernel: my_first_kernel
		./$< 10 1

profiling: profiling.c ze_utils.c | hwv.ar
	$(CC) $^ $(CFLAGS) -o $@

run_profiling: profiling
	./$< 10 1

metric: metric.c ze_utils.c | hwv.ar
	$(CC) $^ $(CFLAGS) -o $@

run_metric: metric
	./$< 10 1

kernel_multithreaded: kernel_multithreaded.c ze_utils.c | hwv.ar
	$(CC) $^ $(CFLAGS) -lpthread -o $@

run_kernel_multithreaded: kernel_multithreaded
	./$< 10 1


SCRIPTS=query_platform.py tiny_clinfo.py my_first_kernel.py verbose.py read_cl.py read_bincl.py buffer_usage.py event_unorder.py event_order.py profiling.py

run: $(addprefix run_, $(SCRIPTS))

run_query_platform.py: query_platform.py
	python $<


run_tiny_clinfo.py: tiny_clinfo.py
	python $<


run_my_first_kernel.py: my_first_kernel.py
	python $< 0 0 10 1


run_verbose.py: verbose.py
	python $< 0 0 10 1


run_read_cl.py: read_cl.py
	python $< 0 0 10 1


hwv.bin: hwv.cl dump_bin.py
	python dump_bin.py 0 0 hwv.cl hwv.bin

run_read_bincl.py: read_bincl.py hwv.bin
	python $< 0 0 10 1

run_buffer_usage.py: buffer_usage.py
	python $< 0 0 10 1

run_event_unorder.py: event_unorder.py
	python $< 0 0 1 1 10

run_event_order.py: event_order.py
	python $< 0 0 1 1 10

run_profiling.py: profiling.py
	python $< 0 0 1 1 10


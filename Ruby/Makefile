
SCRIPTS=platform.rb tiny_clinfo.rb my_first_kernel.rb verbose.rb read_cl.rb read_bincl.rb buffer.rb event_unorder.rb event_order.rb profiling.rb

run: $(addprefix run_, $(SCRIPTS))

run_platform.rb: platform.rb
	ruby $<


run_tiny_clinfo.rb: tiny_clinfo.rb
	ruby $<


run_my_first_kernel.rb: my_first_kernel.rb
	ruby $< 0 0 10 1


run_verbose.rb: verbose.rb
	ruby $< 0 0 10 1


run_read_cl.rb: read_cl.rb
	ruby $< 0 0 10 1


hwv.bin: hwv.cl dump_bin.rb
	ruby dump_bin.rb 0 0 hwv.cl hwv.bin

run_read_bincl.rb: read_bincl.rb hwv.bin
	ruby $< 0 0 10 1

run_buffer.rb: buffer.rb
	ruby $< 0 0 10 1

run_event_unorder.rb: event_unorder.rb
	ruby $< 0 0 1 1 10

run_event_order.rb: event_order.rb
	ruby $< 0 0 1 1 10

run_profiling.rb: profiling.rb
	ruby $< 0 0 1 1 10


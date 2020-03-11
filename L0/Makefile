CC=gcc
CFLAGS=-lze_loader -Wall
BINARIES=tiny_zeinfo
all: $(BINARIES)

run: $(addprefix run_, $(BINARIES))

clean:
	rm -f -- $(BINARIES)
	rm -f -- hwv.bin *.o dump_bin
	
tiny_zeinfo: tiny_zeinfo.c ze_utils.c
	    $(CC) $^ $(CFLAGS) -o $@

run_tiny_zeinfo: tiny_zeinfo
	    ./$<


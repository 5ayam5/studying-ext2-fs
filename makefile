CFLAGS = `pkg-config --cflags fuse` -D_FILE_OFFSET_BITS=64
CLIBS = -lfuse

run: test
	sudo ./test -d -f -s $(device) $(mount)

test: test.c
	gcc $(CFLAGS) test.c $(CLIBS) -o test

clean:
	rm -f test

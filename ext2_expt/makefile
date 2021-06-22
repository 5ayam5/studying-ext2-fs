objects = super group read_inode dir_contents

all: $(objects)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

run-%: %
	sudo ./$< > logs/$<

clean:
	rm -f $(objects)
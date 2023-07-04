include /home/ayan/code/common_flags.mk

list_dir: list_dir.c
	$(CC) $(CFLAGS) list_dir.c -o list_dir

clean: 
	rm list_dir



reader: 
	gcc -ggdb args_support.c queue.h queue.c reader.c -o reader
	
writer: 
	gcc -ggdb args_support.c queue.h queue.c writer.c -lpthread -o writer

all: reader writer
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions reader writer

.PHONY: clean 

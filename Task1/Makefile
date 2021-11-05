
reader: 
	gcc -ggdb reader.c args_support.c -o reader
	
writer: 
	gcc -ggdb writer.c args_support.c -lpthread -o writer

all: reader writer
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions reader writer

.PHONY: clean 

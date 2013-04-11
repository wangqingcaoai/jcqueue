TARGET=jcqueue
dirs = app_queue\
	base_queue\
	secure\
	transfar\
	util\
	parser\
	data\
	
libs = $(dirs:%=lib%.a)
libflag = $(dirs:%=-l% ) 
CFLAGS = -g

$(TARGET):$(libs) main.o build
	$(CC) $(CFLAGS) -L. main.o $(libflag)  -o $@

lib%.a: %/*.c
	$(MAKE) -C  $(@:lib%.a=%)/ CFLAGS=$(CFLAGS)
	ar rs  $@ $(@:lib%.a=%)/*.o 
%.o:%.c
	$(CC) $(CFLAGS) -c $^ 
build:


clean:
	rm -f $(dirs:%=%/*.o)
	rm -f *.a *.tmp
	rm -f *.o
	rm -f $(TARGET)

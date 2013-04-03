TARGET=jcqueue
dirs = data\
	secure\
	transfar\
	util\

	 # app_queue\
	#base_queue\
	
	# parser\
	
libs = $(dirs:%=lib%.a)
libflag = $(dirs:%=-l% ) 
CFLAGS = -g

$(TARGET):$(libs) main.o build
	$(CC) $(CFLAGS) -L. main.o $(libflag)  -o $@

lib%.a: 
	$(MAKE) -C  $(@:lib%.a=%)/ CFLAGS=$(CFLAGS)
	ar rs  $@ `echo $(@:lib%.a=%)/*.o`
%.o:%.c
	$(CC) $(CFLAGS) -c $^ 
build:


clean:
	rm -f $(dirs:%=%/*.o)
	rm -f *.a *.tmp
	rm -f *.o
	rm -f $(TARGET)

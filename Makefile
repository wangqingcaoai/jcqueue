PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
INSTALL=install
TARGET=jcqueue_server jcqueue_client 
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

.PHONY:all
all:$(TARGET)

jcqueue_server:$(libs) jcqueue_server.o 
	$(CC) $(CFLAGS) -L. jcqueue_server.o $(libflag)  -o $@
jcqueue_client:$(libs) jcqueue_client.o 
	$(CC) $(CFLAGS) -L. jcqueue_client.o $(libflag)  -o $@
lib%.a: %/*.c
	$(MAKE) -C  $(@:lib%.a=%)/ CFLAGS=$(CFLAGS)
	ar rs  $@ $(@:lib%.a=%)/*.o 
%.o:%.c
	$(CC) $(CFLAGS) -c $^ 
.PHONY: install
install: $(BINDIR) $(TARGET:%=$(BINDIR)/%)

$(BINDIR):
	$(INSTALL) -d $@

$(BINDIR)/%: %
	$(INSTALL) $< $@

clean:
	rm -f $(dirs:%=%/*.o)
	rm -f *.a *.tmp
	rm -f *.o
	rm -f $(TARGET)

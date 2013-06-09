PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
CONFIG_DIR = /etc/jcq
INSTALL=install
TARGET=jcqueue_server jcqueue_client 
dirs = app_queue\
	base_queue\
	secure\
	transfar\
	parser\
	data\
	util\
	
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
.PHONY: install uninstall
install: $(BINDIR) $(TARGET:%=$(BINDIR)/%) $(CONFIG_DIR) $(TARGET:%=$(CONFIG_DIR)/%.conf)
	
$(BINDIR):
	$(INSTALL) -d $@

$(BINDIR)/%: %
	$(INSTALL) $< $@
$(CONFIG_DIR):
	mkdir $(CONFIG_DIR)
$(CONFIG_DIR)/%:%
	cp $< $@
uninstall:
	rm -f $(TARGET:%=$(BINDIR)/%)

clean:
	rm -f $(dirs:%=%/*.o)
	rm -f *.a *.tmp
	rm -f *.o
	rm -f $(TARGET)

sources = sd-daemon.c\
	server.c\
	connect.c

OS = $(shell uname|tr A-Z a-z)
server_test = server_test.c
CFLAGS = 
target =  server_test
OSFile = $(OS:%=%.o)

default:$(sources:.c=.o) $(OS).o
	


server_test:$(sources:.c=.o) $(server_test:.c=.o) 
	gcc  $(CFLAGS)  $^ -o  $@
# include $(sources:.c=.d)
%.o:%.c
	$(CC) $(CFLAGS)  -c $^

# %.d: %.c
# 	set -e; rm -f $@; \
# 	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
# 	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
# 	rm -f $@.$$$$
clean:
	rm -f *.o $(target) *.d
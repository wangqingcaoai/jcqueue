sourcesFiles = $(wildcard *.c)

OS = $(shell uname|tr A-Z a-z)
server_test = server_test.c
CFLAGS = 
target =  server_test
IgnoreFiles = linux.c freebsd.c  darwin.c %_test.c 
OSFile = $(OS:%=%.o)
sources = $(filter-out $(IgnoreFiles),$(sourcesFiles))
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

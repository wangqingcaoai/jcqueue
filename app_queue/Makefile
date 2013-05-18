
sources = $(wildcard *.c)

CFLAGS = 
default:$(sources:.c=.o) 

# include $(sources:.c=.d)
%.o:%.c
	$(CC) $(CFLAGS)  -c $^

# %.d: %.c
# 	set -e; rm -f $@; \
# 	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
# 	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
# 	rm -f $@.$$$$
clean:
	rm -f *.o  *.d

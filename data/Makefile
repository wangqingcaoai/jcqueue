sourcesFiles = $(wildcard *.c) 
CFLAGS = 
IgnoreFiles = %_test.c 
sources = $(filter-out $(IgnoreFiles),$(sourcesFiles))
default:$(sources:.c=.o)

$(target):$(sources:.c=.o)
	gcc -g $^ -o  $@
# include $(sources:.c=.d)
%.o:%.c
	$(CC) -g  -c $^

# %.d: %.c
# 	set -e; rm -f $@; \
# 	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
# 	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
# 	rm -f $@.$$$$
clean:
	rm -f *.o $(target) *.d

sources = heap.c\
	list.c\
	tree.c\
	mysql.c
target =  list_test
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

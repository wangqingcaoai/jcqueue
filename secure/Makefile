
sources = user.c\

user_test = user_test.c
CFLAGS = 
target =  user_test
default:$(sources:.c=.o) 


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
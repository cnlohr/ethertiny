all : clocktest clocktest2

clocktest : clocktest.o
	gcc -o $@ $^ -lm

clocktest2 : clocktest2.c
	gcc -o $@ $^ -lm

clean :
	rm -rf *.o *~ clocktest

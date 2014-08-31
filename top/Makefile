all : udp udp-histo2

udp-histo2 : udp-histo2.c
	gcc -o $@ $^ -lm

udp : udp.c
	gcc -o $@ $^

clean :
	rm -rf *.o *~ udp udp-histo2

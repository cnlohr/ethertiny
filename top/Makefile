all : udp udp-histo

udp-histo : udp-histo.c
	gcc -o $@ $^ -lm

udp : udp.c
	gcc -o $@ $^

clean :
	rm -rf *.o *~ udp udp-histo

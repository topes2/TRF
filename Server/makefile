ser: bin/main.o bin/network.o
	gcc -o $@ $^

bin/main.o: ServerTCP.c | network.h bin
	gcc -c -o $@ $^
bin/network.o: network.c | network.h bin
	gcc -c -o $@ $^
	
bin:
	mkdir bin
clean:
	rm -rf bin client
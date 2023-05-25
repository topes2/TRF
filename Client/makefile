cli: bin/main.o bin/network.o bin/funcs.o
	gcc -o $@ $^

bin/main.o: ClientTCP.c | network.h funcs.o bin
	gcc -c -o $@ $^
bin/network.o: network.c | network.h bin
	gcc -c -o $@ $^
bin/funcs.o: funcs.c | funcs.h bin
	gcc -c -o $@ $^

bin:
	mkdir bin
clean:
	rm -rf bin client
cli: bin/main.o
	gcc -o $@ $^
bin/main.o: ClientTCP.c | bin
	gcc -c -o $@ $^
bin:
	mkdir bin
clean:
	rm -rf bin client
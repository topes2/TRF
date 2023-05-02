ser: bin/main.o
	gcc -o $@ $^
bin/main.o: ServerTCP.c | bin
	gcc -c -o $@ $^
bin:
	mkdir bin
clean:
	rm -rf bin client
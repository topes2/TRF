main: bin/main.o
	gcc -o $@ $^ 

bin/main.o: main.c | bin
	gcc -c -o $@ $^ 

bin:
	mkdir bin

clean:
	rm -rf bin main
main: bin/main.o
	gcc -o $@ $^ -lgdbm 
bin/main.o: Main.c | bin
	gcc -c -o $@ $^

bin:
	mkdir bin
clean:
	rm -rf bin Main
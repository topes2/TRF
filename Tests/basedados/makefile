main: bin/main.o
	gcc -o $@ $^ -lgdbm 
bin/main.o: BaseDados.c | bin
	gcc -c -o $@ $^

bin:
	mkdir bin
clean:
	rm -rf bin Main
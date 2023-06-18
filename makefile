all: clean ser/ser cli/cli

ser/ser: bin/serTCP.o bin/ser/network.o bin/ser/db.o bin/ser/funcs.o bin/ser/qa.o bin/ser/files.o
	gcc -o $@ $^ -lgdbm
cli/cli: bin/cliTCP.o bin/cli/network.o bin/cli/funcs.o
	gcc -o $@ $^


bin/serTCP.o: ser/serTCP.c | bin configs.h ser/network/network.h ser/funcs/funcs.h
	gcc -c -o $@ $^
bin/cliTCP.o: cli/cliTCP.c | bin configs.h cli/network/network.h cli/funcs/funcs.h
	gcc -c -o $@ $^

	
bin/cli/funcs.o: cli/funcs/funcs.c | bin/cli configs.h cli/funcs/funcs.h cli/network/network.h
	gcc -c -o $@ $^
bin/cli/network.o: cli/network/network.c | bin/cli configs.h cli/network/network.h
	gcc -c -o $@ $^
	
bin/ser/db.o: ser/databases/db.c | bin/ser ser/databases/db.h
	gcc -c -o $@ $^
bin/ser/network.o: ser/network/network.c | bin/ser configs.h ser/network/network.h
	gcc -c -o $@ $^
bin/ser/funcs.o: ser/funcs/funcs.c | bin/ser configs.h ser/funcs/funcs.h ser/databases/db.h
	gcc -c -o $@ $^ 
bin/ser/qa.o: ser/qa/questan.c | bin/ser ser/qa/questan.h ser/databases/db.h ser/network/network.h
	gcc -c -o $@ $^
bin/ser/files.o: ser/files/files.c | bin/ser ser/files/files.h configs.h ser/databases/db.h ser/network/network.h
	gcc -c -o $@ $^ 


bin/ser:
	mkdir bin/ser
bin/cli:
	mkdir bin/cli
bin:
	mkdir bin


clean:
	rm -rf bin cli/cli ser/ser
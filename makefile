all: clean ser cli

ser: bin/serverTCP.o bin/ser/network.o bin/ser/backup.o bin/ser/db.o bin/ser/funcs.o
	gcc -o $@ $^ -lgdbm
cli: bin/clientTCP.o bin/cli/network.o bin/cli/funcs.o
	gcc -o $@ $^


bin/serverTCP.o: Server/serverTCP.c | bin configs.h Server/network/network.h
	gcc -c -o $@ $^
bin/clientTCP.o: Client/clientTCP.c | bin configs.h Client/network/network.h Client/funcs/funcs.h
	gcc -c -o $@ $^

	
bin/cli/funcs.o: Client/funcs/funcs.c | bin/cli Client/funcs/funcs.h
	gcc -c -o $@ $^
bin/cli/network.o: Client/network/network.c | bin/cli Client/network/network.h configs.h
	gcc -c -o $@ $^

	
bin/ser/backup.o: Server/backup/backup.c | bin/ser Server/backup/backup.h 
	gcc -c -o $@ $^
bin/ser/db.o: Server/databases/db.c | bin/ser Server/databases/db.h
	gcc -c -o $@ $^
bin/ser/funcs.o: Server/funcs/funcs.c | bin/ser Server/funcs/funcs.h Server/network/network.h configs.h Server/databases/db.h
	gcc -c -o $@ $^
bin/ser/network.o: Server/network/network.c | bin/ser Server/network/network.h configs.h Server/funcs/funcs.h
	gcc -c -o $@ $^



bin/ser:
	mkdir bin/ser
bin/cli:
	mkdir bin/cli
bin:
	mkdir bin


clean:
	rm -rf bin cli ser
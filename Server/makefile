ser: bin/main.o bin/network.o bin/backup.o bin/db.o
	gcc -o $@ $^ -lgdbm

bin/main.o: ServerTCP.c | network.h backup/backup.h database/db.h defines.h bin
	gcc -c -o $@ $^
bin/network.o: network.c | network.h bin
	gcc -c -o $@ $^
bin/backup.o: backup/backup.c | backup/backup.h bin
	gcc -c -o $@ $^ 
bin/db.o: database/db.c | database/db.h bin
	gcc -c -o $@ $^

bin:
	mkdir bin
clean:
	rm -rf bin ser
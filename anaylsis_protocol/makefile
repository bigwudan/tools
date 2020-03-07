a.out: main.o analysis_protocol.o
	gcc main.o analysis_protocol.o -o a.out

main.o: main.c
	gcc -c main.c -o main.o

analysis_protocol.o: analysis_protocol.c analysis_protocol.h
	gcc -c analysis_protocol.c -o analysis_protocol.o

clean:
	rm -rf main analysis_protocol.o



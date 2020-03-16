a.out: main.o analysis_protocol.o analysis_protocol_tools.o heap_2.o
	gcc -g3 main.o analysis_protocol.o analysis_protocol_tools.o heap_2.o -o a.out

main.o: main.c
	gcc -g3 -c main.c -o main.o

analysis_protocol.o: analysis_protocol.c analysis_protocol.h
	gcc -g3 -c analysis_protocol.c -o analysis_protocol.o

analysis_protocol_tools.o: analysis_protocol_tools.c analysis_protocol_tools.h
	gcc -g3 -c analysis_protocol_tools.c -o analysis_protocol_tools.o
heap_2.o: heap_2.c heap.h
	gcc -g3 -c heap_2.c -o heap_2.o
clean:
	rm -rf main analysis_protocol.o analysis_protocol_tools.o



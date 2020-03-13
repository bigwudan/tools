a.out: main.o analysis_protocol.o analysis_protocol_tools.o
	gcc -g main.o analysis_protocol.o analysis_protocol_tools.o -o a.out

main.o: main.c
	gcc -g -c main.c -o main.o

analysis_protocol.o: analysis_protocol.c analysis_protocol.h
	gcc -g -c analysis_protocol.c -o analysis_protocol.o

analysis_protocol_tools.o: analysis_protocol_tools.c analysis_protocol_tools.h
	gcc -g -c analysis_protocol_tools.c -o analysis_protocol_tools.o
clean:
	rm -rf main analysis_protocol.o analysis_protocol_tools.o



MY_DIR = ../

test_main.out: test_main.o analysis_protocol.o analysis_protocol_tools.o heap_2.o
	gcc -g3 test_main.o analysis_protocol.o analysis_protocol_tools.o heap_2.o -o test_main.out
test_main.o: test_main.c
	gcc -g3 -c test_main.c -o test_main.o
analysis_protocol_tools.o: $(MY_DIR)analysis_protocol_tools.c $(MY_DIR)analysis_protocol_tools.h
	gcc -g3 -c $(MY_DIR)analysis_protocol_tools.c -o analysis_protocol_tools.o
analysis_protocol.o: $(MY_DIR)analysis_protocol.c $(MY_DIR)analysis_protocol.h
	gcc -g3 -c $(MY_DIR)analysis_protocol.c -o analysis_protocol.o
heap_2.o: $(MY_DIR)heap_2.c $(MY_DIR)heap.h
	gcc -g3 -c $(MY_DIR)heap_2.c -o heap_2.o
clean:
	rm -rf *.o

#!/bin/bash
gcc -g3 -std=c99 fastcgi.c wl_buffer.c test.c -lcmockery -o test

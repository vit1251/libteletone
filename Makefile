
CFLAGS=-Iinclude -lm -L.

all: compile samples

compile:
	$(CC) $(CFLAGS) -c -o lib/libteletone_detect.o src/libteletone_detect.c
	$(CC) $(CFLAGS) -c -o lib/libteletone_generate.o src/libteletone_generate.c
	#$(CC) $(CFLAGS) -c -o lib/libteletone_generate_util.o src/libteletone_generate_util.c
	ar cr libteletone.a lib/libteletone_generate.o lib/libteletone_detect.o

samples:
	$(CC) $(CFLAGS) -o bin/1 samples/detect_sample.c -lteletone
	$(CC) $(CFLAGS) -o bin/2 samples/generate_sample.c -lteletone


.PHONY: all compile samples



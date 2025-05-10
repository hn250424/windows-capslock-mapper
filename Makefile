CC = gcc
CFLAGS = -Wall -O2 -I./src/constants -I./src/utils
TARGETS = cm.exe cm_runner.exe

all: $(TARGETS)

build/constants/app_constants.o: src/constants/app_constants.c src/constants/app_constants.h
	$(CC) $(CFLAGS) -c src/constants/app_constants.c -o build/constants/app_constants.o

build/constants/key_constants.o: src/constants/key_constants.c src/constants/key_constants.h
	$(CC) $(CFLAGS) -c src/constants/key_constants.c -o build/constants/key_constants.o

build/constants/result_constants.o: src/constants/result_constants.c src/constants/result_constants.h
	$(CC) $(CFLAGS) -c src/constants/result_constants.c -o build/constants/result_constants.o

build/utils/common.o: src/utils/common.c src/utils/common.h
	$(CC) $(CFLAGS) -c src/utils/common.c -o build/utils/common.o

build/utils/mutex.o: src/utils/mutex.c src/utils/mutex.h
	$(CC) $(CFLAGS) -c src/utils/mutex.c -o build/utils/mutex.o

build/utils/env.o: src/utils/env.c src/utils/env.h
	$(CC) $(CFLAGS) -c src/utils/env.c -o build/utils/env.o

build/utils/registry.o: src/utils/registry.c src/utils/registry.h
	$(CC) $(CFLAGS) -c src/utils/registry.c -o build/utils/registry.o

build/process.o: src/process.c src/process.h
	$(CC) $(CFLAGS) -c src/process.c -o build/process.o

cm.exe: src/main.c build/process.o build/utils/common.o build/utils/mutex.o build/utils/env.o build/utils/registry.o build/constants/app_constants.o build/constants/key_constants.o build/constants/result_constants.o
	$(CC) $(CFLAGS) -o build/cm.exe src/main.c build/process.o build/utils/common.o build/utils/mutex.o build/utils/env.o build/utils/registry.o build/constants/app_constants.o build/constants/key_constants.o build/constants/result_constants.o

cm_runner.exe: src/runner.c build/process.o build/utils/common.o build/utils/mutex.o build/utils/env.o build/utils/registry.o build/constants/app_constants.o build/constants/key_constants.o build/constants/result_constants.o
	$(CC) $(CFLAGS) -o build/cm_runner.exe src/runner.c build/process.o build/utils/common.o build/utils/mutex.o build/utils/env.o build/utils/registry.o build/constants/app_constants.o build/constants/key_constants.o build/constants/result_constants.o

clean:
	del /Q build\*.exe build\*.o

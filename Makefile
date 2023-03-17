CC = g++
CFLAGS = -Wall -Werror
LIBS = -lc -lstdc++
OBJS = files.o diff.o history.o lock.o main.o
OUT = vcom

all: $(OUT)

vcom: files.o diff.o history.o lock.o main.o 
	$(CC) $(CFLAGS) -o $(OUT) $(OBJS) $(LIBS)

history.o: history.c history.h
	$(CC) $(CFLAGS) -c history.c

files.o: files.c files.h
	$(CC) $(CFLAGS) -c files.c

lock.o: lock.c lock.h
	$(CC) $(CFLAGS) -c lock.c

diff.o: diff.cpp diff.h
	$(CC) $(CFLAGS) -c diff.cpp

main.o: main.c files.h diff.h lock.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f prog *.o

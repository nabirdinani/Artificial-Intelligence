CC=g++-4.7
CFLAGS= $(fltk-config --use-images  --compile) -c -std=c++11

all: negation adder

negation: nonGA_Neg.o
	$(CC) $(fltk-config --use-images  --compile) nonGA_Neg.o -o negation -lfltk -lX11
nonGA_Neg.o: *.cpp
	$(CC) $(CFLAGS) nonGA_Neg.cpp
adder: nonGA_Add.o
	$(CC) $(fltk-config --use-images  --compile) nonGA_Add.o -o adder -lfltk -lX11
nonGA_Add.o: *.cpp
	$(CC) $(CFLAGS) nonGA_Add.cpp
clean:
	rm *.o negation adder
default: all

all: mnist2pgm test_mnist

test: test_mnist
	./test_mnist

mnist2pgm: mnist2pgm.o mnist.o
	gcc -g -std=c11 -Wall -Werror -pedantic mnist2pgm.o mnist.o -o mnist2pgm

mnist2pgm.o:
	gcc -g -std=c11 -Wall -Werror -pedantic -c mnist2pgm.c

valgrind: test_mnist
	valgrind --show-reachable=yes --leak-check=full --track-origins=yes -v ./test_mnist

test_mnist: mnist.o test_mnist.o
	gcc -g -std=c11 -Wall -Werror -pedantic test_mnist.o mnist.o -o test_mnist -lcunit -lgmp

mnist.o: mnist.c mnist.h
	gcc -g -std=c11 -Wall -Werror -pedantic -c mnist.c

test_mnist.o: test_mnist.c
	gcc -g -std=c11 -Wall -Werror -pedantic -c test_mnist.c

clean:
	rm -f *.o
	rm -f test_mnist
	rm -f mnist2pgm


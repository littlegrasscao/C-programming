default: arb_test

check: arb_unittest
	./arb_unittest

valgrind: arb_test
	valgrind --show-reachable=yes --leak-check=full --track-origins=yes -v ./arb_test

arb_test: arb_test.o arb_int.o
	gcc -g -std=c11 -Wall -Werror -pedantic arb_test.o arb_int.o -o arb_test

arb_unittest: arb_unittest.o arb_int.o
	gcc -g -std=c11 -Wall -Werror -pedantic arb_unittest.o arb_int.o -o arb_unittest -lcunit -lgmp

arb_unittest.o: arb_unittest.c
	gcc -g -std=c11 -Wall -Werror -pedantic -c arb_unittest.c

arb_test.o: arb_test.c
	gcc -g -std=c11 -Wall -Werror -pedantic -c arb_test.c

arb_int.o: arb_int.c arb_int.h
	gcc -g -std=c11 -Wall -Werror -pedantic -c arb_int.c

clean:
	rm -f *.o
	rm -f arb_test
	rm -f arb_unittest

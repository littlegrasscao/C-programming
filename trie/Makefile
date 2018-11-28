default: test

test: trie_test
	./trie_test

valgrind: trie_test
	valgrind --show-reachable=yes --leak-check=full --track-origins=yes -v ./trie_test

trie_test: trie.o trie_test.o
	gcc -g -std=c11 -Wall -Werror -pedantic trie_test.o trie.o -o trie_test -lcunit -lgmp

trie.o: trie.c trie.h
	gcc -g -std=c11 -Wall -Werror -pedantic -c trie.c

trie_test.o: trie_test.c
	gcc -g -std=c11 -Wall -Werror -pedantic -c trie_test.c

clean:
	rm -f *.o
	rm -f trie_test

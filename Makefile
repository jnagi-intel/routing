all: routing ssp

ssp: SspMain.o Ssp.o FibHeap.o
	gcc -Wall -o ssp SspMain.o Ssp.o FibHeap.o -lm

routing: RoutingMain.o Routing.o Ssp.o Trie.o FibHeap.o
	gcc -Wall -o routing RoutingMain.o Routing.o Ssp.o Trie.o FibHeap.o -lm

RoutingMain.o: RoutingMain.c
	gcc -Wall -c RoutingMain.c

SspMain.o: SspMain.c
	gcc -Wall -c SspMain.c

Routing.o: Routing.c
	gcc -Wall -c Routing.c

Ssp.o: Ssp.c
	gcc -Wall -c Ssp.c

Trie.o:Trie.c
	gcc -Wall -c Trie.c

FibHeap.o: FibHeap.c
	gcc -Wall -c FibHeap.c

clean: 
	rm -rf routing ssp *.o *~


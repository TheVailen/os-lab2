all: parent child

parent: parent.c
	gcc -Wall -Wextra parent.c -o parent

child: child.c
	gcc -Wall -Wextra child.c -o child

clean:
	rm -f parent child

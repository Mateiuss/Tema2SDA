build: main.c functions.c
	gcc -g -Wall -m32 -std=c99 -o tema2 main.c functions.c functiiCoada.c functiiStiva.c
clean:
	rm tema2

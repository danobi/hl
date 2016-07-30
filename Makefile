all: hl

hl: hl.c
	$(CC) -o hl hl.c -Wall -W -pedantic -std=c11 -g

clean:
	rm hl

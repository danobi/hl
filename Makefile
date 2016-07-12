all: hl

kilo: hl.c
	$(CC) -o hl hl.c -Wall -W -pedantic -std=c11

clean:
	rm hl

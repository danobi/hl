all: hl

hl: hl.cc
	$(CXX) -o hl hl.cc -Wall -W -pedantic -std=c++11 -g

install: all
	@cp -f hl ~/.local/bin

uninstall:
	@rm -f ~/.local/bin/hl

clean:
	rm hl

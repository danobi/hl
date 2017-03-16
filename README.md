hl
======
hl: a text stream highlighter.

USAGE: hl -r <regex> [options] [file]

Options:

  -r <regex>       Set regex to highlight. Only POSIX basic regexs are
                   supported right now.
  -f <file>        Specify file to highlight and print to stdout. If a file
                   is not specified, hl will read from stdin.
  -h               Display this message and exit.

#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

#define COLOR_MATCH "\033[30;43m"  // black with yellow background
#define END_COLOR "\x1b[0m"

const char usage[] = R"(hl: a text stream highlighter.

USAGE: hl -r <regex> [options] [file]

Options:

  -r <regex>       Set regex to highlight. Only POSIX basic regexs are
                   supported right now.
  -f <file>        Specify file to highlight and print to stdout. If a file
                   is not specified, hl will read from stdin.
  -h               Display this message and exit.
)";

using namespace std;

bool   f_from_file = false;      // true if getting stream from file
string o_filename;               // name of file
string o_regex;                  // regex to match

int print_help()
{
  cout << usage << endl;
  exit(EXIT_SUCCESS);
}

int process_args(int argc, char **argv)
{
  int c;
  while ((c = getopt(argc, argv, "hf:r:")) != -1) {
    switch (c) {
    case 'h':
      return print_help();
      break;
    case 'r':
      o_regex = string(optarg);
      break;
    case 'f':
      f_from_file = true;
      o_filename = string(optarg);
      break;
    case '?':
      if (optopt == 'r' || optopt == 'f') {
        cerr << "Option -" << optopt << " needs an argument" << endl;
        return 1;
      }
      break;
    }
  }

  // if we don't have a regex to match, then we have to quit
  if (!o_regex.size()) {
    cerr << "Missing -r <regex> option" << endl;
    return 1;
  }

  // any left over cmd line args must be a passed in file name
  if (optind < argc) {
    f_from_file = true;
    o_filename = string(argv[optind]);
  }

  return 0;
}

void parse_highlight_print(string &line, string &rgx)
{
  smatch matches;
  regex pat(rgx, regex::basic);

  string new_str(regex_replace(line, pat, COLOR_MATCH "$&" END_COLOR));
  cout << new_str << endl;;
}

void read_parse_loop(istream &stream, string &rgx)
{
  string line;
  while (getline(stream, line)) {
    parse_highlight_print(line, rgx);
  }
}

int main(int argc, char **argv)
{
  int ret;
  if ((ret = process_args(argc, argv))) {
    return ret;
  }

  /*
  cout << "f_from_file: " << f_from_file << endl;
  cout << "o_filename: " << o_filename << endl;
  cout << "o_regex: " << o_regex << endl;
  */

  if (f_from_file) {
    ifstream input(o_filename, ifstream::in);
    if (input.good()) {
      read_parse_loop(input, o_regex);
    } else {
      cerr << "Bad file: " << o_filename << endl;
      return EXIT_FAILURE;
    }
  } else {
    read_parse_loop(cin, o_regex);
  }
}

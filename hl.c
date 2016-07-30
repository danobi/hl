#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/select.h>

#define INIT_BUF_SIZE 1024

// forward declares
void process_args(int argc, char** argv);
void exit_success();
int32_t exists_newline(uint8_t* buf);
void read_parse_loop(int fd, uint8_t* pattern);
void parse_highlight_print(uint8_t* buf, uint8_t* pattern, uint8_t n);


// flags
bool f_from_stdin = false;     // true if getting stream from stdin
bool f_from_file = false;      // true if getting stream from file
char* o_filename = NULL;       // name of file

void process_args(int argc, char** argv)
{
    // no args
    if (argc == 1) {
        f_from_stdin = true;
    } else if (argc > 1) {
        f_from_file = true;
        o_filename = argv[1];
    }
    // TODO: provide -h flag and use optparse
}

/*
 * Terminates this progoram after a successful execution
 */
void exit_success()
{
    fprintf(stdout, "\n");  // so terminal prompt doesn't get messed up
    exit(EXIT_SUCCESS);
}


/*
 * Checks for the existence of a new line in buf
 *
 * @param buf a nul terminated buffer to be checked
 * @return the index to the location of the newline if found, NULL elsewise
 */
int32_t exists_newline(uint8_t* buf)
{
    uint8_t* buf_start = buf;

    if (buf == NULL)
        return -1;

    while (*buf) {
        if (*buf == '\n')
            return (buf - buf_start);
        ++buf;
    }

    return -1;
}


/*
 * Takes a in a line then highlights the given pattern and prints the highlighted output to stdout
 *
 * @param buf is a buffer containing at least the desired string to be highlighted (could contain more than that
 * @param pattern is a basic POSIX regex
 * @param n is the number of elements to consider
 */
void parse_highlight_print(uint8_t* buf, uint8_t* pattern, uint8_t n)
{
    // stub
    // TODO: implemnet
    uint8_t tempbuf[n + 1];
    memcpy(tempbuf, buf, n);
    tempbuf[n] = '\0';
    printf("%s", tempbuf);
    return;
}


/*
 * Main loop: reads input and parses line by line to find pattern
 *
 * @param fd a fd to receive input from
 * @param pattern POSIX basic regex pattern to highlight
 */
void read_parse_loop(int fd, uint8_t* pattern)
{
    fd_set set;
    struct timeval tv;
    uint32_t buf_in_use = 0;
    uint32_t cur_buf_size = INIT_BUF_SIZE;
    uint32_t next_empty_idx = 0;
    uint8_t* buf = malloc(INIT_BUF_SIZE * sizeof(char));

    while (1) {
        FD_ZERO(&set);
        FD_SET(fd, &set);

        // nonblocking select call
        tv.tv_sec = 0;
        tv.tv_usec = 1;

        int32_t retval = select(fd + 1, &set, NULL, NULL, &tv);

        if (retval == -1)
            perror("select()\n");
        else if (FD_ISSET(fd, &set)) {
            ssize_t bytes_read;

            // resize buffer if necessary
            if (cur_buf_size - next_empty_idx < INIT_BUF_SIZE) {
                buf = realloc(buf, cur_buf_size + INIT_BUF_SIZE);
                cur_buf_size += INIT_BUF_SIZE;
            }

            // read into buf
            if ((bytes_read = read(fd, buf + next_empty_idx, cur_buf_size - next_empty_idx - 1)) <= 0
                    && (buf_in_use) <= 0)
                exit_success();

            buf_in_use += bytes_read;
            next_empty_idx += bytes_read;
            buf[next_empty_idx] = '\0';

            // scan for newline
            int32_t newline_idx = exists_newline(buf);
            if (newline_idx < 0) {
                // keep reading input
                continue;
            } else {
                // newline found
                parse_highlight_print(buf, pattern, newline_idx + 1);
                uint8_t leftover_size =  next_empty_idx - newline_idx - 1;

                // copy over all text after newline to beginning of a new buffer
                uint8_t* new_buf = malloc(INIT_BUF_SIZE * sizeof(uint8_t));
                memcpy(new_buf, buf + newline_idx + 1, leftover_size);
                free(buf);
                buf = new_buf;
                buf_in_use -= newline_idx + 1;

                // we were allowed to make new_buf size INIT_BUF_SIZE large because we
                // incremented the size of the buf by INIT_BUF_SIZE each time, so the
                // max leftover size is INIT_BUF_SIZE - 1 elemnets
            }
        } else
            exit_success();

    }
    free(buf);
}


int main(int argc, char** argv)
{
    int fd;
    process_args(argc, argv);

    if (f_from_stdin)
        fd = STDIN_FILENO;
    else if (f_from_file) {
        fd = open(o_filename, O_RDONLY);
        if (fd < 0)
            perror("open() failed");
    }

    // TODO: actually give the pattern and not just NULL
    read_parse_loop(fd, NULL);
}

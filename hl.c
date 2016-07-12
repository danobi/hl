#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>

#define BUF_SIZE 1024

int main(int argc, char* argv)
{
    fd_set set;
    char buf[BUF_SIZE];

    while (1) {
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set); 

        int retval = select(STDIN_FILENO + 1, &set, NULL, NULL, NULL); 

        if (retval == -1) {
            perror("select()\n");
        } else if (FD_ISSET(STDIN_FILENO, &set)){
            ssize_t bytes_read;

            // if there's no more input, exit
            if ((bytes_read = read(STDIN_FILENO, buf, BUF_SIZE)) <= 0) {
                fprintf(stdout, "\n");  // so terminal prompt doesn't get messed up
                exit(EXIT_SUCCESS);
            } else {
                buf[bytes_read] = '\0';
            }

            fprintf(stdout, "%s", buf);
        }
    }
}

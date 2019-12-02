#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    /* 
    * disable Ctrl-S and Ctrl-Q and Fix Ctrl-M
    * BRKINT : send a SIGINT signal to the program
    * INPCK : enable parity checking
    */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // turn off "\n" and "\r\n"
    raw.c_oflag &= ~(OPOST);
    /*
        * CS8 : set character size to 8 bits per byte
        */
    raw.c_cflag |= (CS8);
    // disable ECHO Canonical Ctrl-C Ctrl-Z and Ctr-V
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main() {
    
    enableRawMode();
    while (1) {
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') break;
    }
    return 0;
}

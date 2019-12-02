#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
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
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
  }
    return 0;
}

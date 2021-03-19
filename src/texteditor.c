#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios original_termios;

// terminate &/ error handling
void die(const char *s) {
    perror(s);
    exit(1);
} 

void disableTerminalRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
        die("tcsetattr");
}

void enableTerminalRawMode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
        die("tcgetattr");

    atexit(disableTerminalRawMode);

    struct termios raw = original_termios;
    // disable CTRL+S and CTRL+Q
    raw.c_lflag &= ~(IXON | ICRNL); 
    // disable \n and \r\n
    // POST means Post-processing output. OUTPUT flag -> comes from <termios.h>
    raw.c_lflag &= ~(OPOST); 
    // disable CTRL+C, CTRL+Z, CTRL+Y, CTRL+V
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // disable misc flags
    raw.c_cflag |= (CS8);
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP);

    // adding input timeout
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // enable raw
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetarr");
}

char editorReadKeyStrokes() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
        if (nread == -1 && errno != EAGAIN) die("read");
    return c;
}

void editorProcessKeyStrokes() {
    char c = editorReadKeyStrokes();

    switch (c) {
    case CTRL_KEY('q'):
        exit(0);
        break;
    }
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

int main() {
    enableTerminalRawMode();

    char c;
    while (1) {
        editorRefreshScreen();
        editorProcessKeyStrokes();
    }

    return 0;
}
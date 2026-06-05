#include <stdio.h>
#include "terminal.h"

void set_mode(int enable_raw)
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    
    if (enable_raw) {
        t.c_lflag &= ~(ICANON | ECHO); 
        t.c_cc[VMIN] = 1;
        t.c_cc[VTIME] = 0;
    } else {
        t.c_lflag |= (ICANON | ECHO); 
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
char get_c()
{
    char c;
    set_mode(1);
    while (1)
    {
        c = getchar();
        if (c == 'y' || c == 'n') break; 
    }
    set_mode(0);
    return c;
}
void clear()
{
    printf("\33[H\33[2J");
}

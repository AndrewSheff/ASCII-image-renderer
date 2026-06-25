#include <stdio.h>
#include "terminal.h"

void set_mode(int enable_raw) // Switch CANONICAL and NON-CANONICAL input
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    
    if (enable_raw) { // NON-CANONICAL
        t.c_lflag &= ~(ICANON | ECHO); // ICANON = false, ECHO = false
        t.c_cc[VMIN] = 1; // How many chars need to read buffer
        t.c_cc[VTIME] = 0; // Freeze between you press the button and buffer reads

    } else { // CANONICAL
        t.c_lflag |= (ICANON | ECHO); // ICANON = true, ECHO = true 
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

char get_c() // Read user short answer
{
    char c;
    set_mode(1); // Disable writing user's "y" or "n" in terminal
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

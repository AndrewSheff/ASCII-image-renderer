#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>
#include <unistd.h>

#define GINGER "\e[38;2;255;160;0m"
#define GREEN "\e[38;2;0;255;0m"
#define RED "\e[38;2;255;0;0m"

void set_mode(int enable_raw); // Switch CANONICAL and NON-CANONICAL input
char get_c(); // Read user short answer
void clear();


#endif
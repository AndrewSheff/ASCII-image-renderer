#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h> // For deep work with system
// #include <unistd.h>
#include <dirent.h> // For work with directories
#include <termios.h> // For work with terminal

#include "terminal.h"

struct file_header
{
    char format[2]; // Magic number - image format - ASCII(P3) or binary(P6)
    int columns;
    int rows;
    int brightness; // Max brightness for one channel (R, G, B). Standart is 255
};

struct color // RGB color value for pixel
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct file_header get_header(FILE *file) // Read file headers
{
    struct file_header file_header;
    fscanf(file, "%s%d%d%d", file_header.format, &file_header.columns, &file_header.rows, &file_header.brightness); 
    // Example:              "P6",               736,                  858,               255
    return file_header;
}

void get_colors(FILE *file, struct file_header file_header, struct color colors[file_header.rows][file_header.columns]) // Read and put pixels colors in 2D array
{
    for (int r=0; r<file_header.rows; r++)
    {
        for (int c=0; c<file_header.columns; c++)
        {
            struct color color;
            for (int i=0; i<=2; i++) // Iterate pixels values
            {
                switch (i)
                {
                    case 0:
                    {
                        color.blue = fgetc(file); // Read B value...
                        break;
                    }
                    case 1:
                    {
                        color.red = fgetc(file); // Read R value...
                        break;
                    }
                    case 2:
                    {
                        color.green = fgetc(file); // Read G value...
                        break;
                    } // ...as a char
                }
            }
            colors[r][c] = color; // Save pixel
        }
    }
}

void postprocess(float c_divider, float r_divider, int w_row, int w_col, 
    struct color colors[w_row][w_col], struct file_header file_header, struct color original_colors[file_header.rows][file_header.columns], 
    int interpolation)
{
    for (int w_r=0; w_r<w_row; w_r++)
    {
        for (int w_c=0; w_c<w_col; w_c++)
        {
            // Nearest neighbor method
            int r = w_r/r_divider; // Example: <first row> / r_divider
            int c = w_c/c_divider; // Example: <first column> / c_divider
            struct color i_color = original_colors[r][c];

            if (interpolation)
            {
                int red = 0;
                int green = 0;
                int blue = 0;
                int count = 0;

                for (int i=0; i<file_header.rows/w_row; i++)
                {
                    // Check ...

                    if (r-i>=0) // ...left - i neighbor...
                    {
                        count++;
                        red = red + original_colors[r-i][c].red;
                        green = green + original_colors[r-i][c].green;
                        blue = blue + original_colors[r-i][c].blue;
                    }
                    if (r+i<=file_header.rows) // ...right + i neighbor...
                    {
                        count++;
                        red = red + original_colors[r+i][c].red;
                        green = green + original_colors[r+i][c].green;
                        blue = blue + original_colors[r+i][c].blue;
                    }
                    if (c-i>=0) // ...up - i neighbor...
                    {
                        count++;
                        red = red + original_colors[r][c-i].red;
                        green = green + original_colors[r][c-i].green;
                        blue = blue + original_colors[r][c-i].blue;
                    }
                    if (c+i<=file_header.columns) // ...down + i neighbor...
                    {
                        count++;
                        red = red + original_colors[r][c+i].red;
                        green = green + original_colors[r][c+i].green;
                        blue = blue + original_colors[r][c+i].blue;
                    } // ...pixel
                }

                i_color.red = red / count;
                i_color.green = green / count;
                i_color.blue = blue / count;
            }
            colors[w_r][w_c] = i_color;
        }
    }
}

// -------------------------------------------------------------------
// TERMINAL PARAMS OPTIONS

void choose_file(char* filename)
{
    char **names = NULL;
    printf("Here is .ppm images in this directory, choose one \n");
    DIR *dir = opendir(".");
    struct dirent *entry;
    int i = 0;

    while ((entry = readdir(dir)) != NULL) // As long as there are files in the directory
    {
        if (strstr(entry->d_name, ".ppm")) // If file in format .ppm
        {
            printf("[%d] %s\n", i, entry->d_name); 
            // Example: [0] test.ppm
            char **r_names = realloc(names, i*sizeof(char*) + sizeof(char*));

            if (r_names)
            {
                names = r_names;
                names[i] = strdup(entry->d_name);
                i++;
            }
            else
            {
                printf("Segmentation fault\n"); // The best error handling in the world
                return;
            }
        }
    }

    printf("Type index: ");
    int number;
    scanf("%d" , &number);
    if (number > i)
    {
        printf("Didnt find file with %d index\n", number);
        return;
    }
    else
    {
        strcpy(filename,names[number]);
    }
}

void choose_size(int* width, int* height, short* proportions)
{
    printf("Output image size\n");
    printf("Width: ");
    scanf("%d" , width);
    printf("Height: ");
    scanf("%d" , height);
    printf("Save original proportions? [y/N] ");
    fflush(stdout);
    char c = get_c();
    *proportions = c=='y';
}

short choose_inter()
{
    printf("Interpolation? [y/N] ");
    fflush(stdout);
    return get_c()=='y';
}

// -------------------------------------------------------------------

int main(int argc, char *argv[]) // argc - number of args, argv[] - themselves
{

    printf("\33[s"); // Save text cursor position in terminal
    fflush(stdout); // Clear buffer

    char* filename = malloc(1024); 
    filename[0] = '\n';

    // Terminal writing params

    short is_inter = 0;
    int width = 0; // Cols
    int height = 0; // Rows
    short proportions = 1;

    if (argc==1) // No args - run interactive menu
    {
        choose_file(filename);
        printf("\n");
        choose_size(&width, &height, &proportions);
        printf("\n");
        is_inter = choose_inter();
        printf("\n");
    }

    if (argc>1)
    {
        for (int i = 1; i<argc; i++)
        {
            // strstr() - find first occurrence of the second string in the first

            if (strstr(argv[i],"--inter"))
            {
                is_inter = 1;
            }
            if (strstr(argv[i],".ppm")) // User gets file name
            {
                filename = argv[i];
            }
            if (strstr(argv[i], "--width"))
            {
                i++;
                width = atoi(argv[i]);
            }
            else if (strstr(argv[i], "--height"))
            {
                i++;
                height = atoi(argv[i]);
            }
            else if (strstr(argv[i], "--no-proportion"))
            {
                proportions = 0;
            }
        }
    }

    if (filename[0]=='\n')
    {
        printf("filename is NULL\n");
        return 1;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return 1;
    }

    /* Read file */

    struct file_header file_header = get_header(file);
    struct color original_colors[file_header.rows][file_header.columns];
    get_colors(file, file_header, original_colors);

    if (width==0 && height==0) // Scan terminal params
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
        height = w.ws_row;
    }
    short is_width = width!=0; // Is width, ...
    short is_height = height!=0; // ...height exists
    float c_divider = (float)(width)/file_header.columns; // Rect width
    float r_divider = (float)(height)/file_header.rows; // Rect height

    if (proportions || !is_width || !is_height)
    {
        if ((is_width = is_height))
        {
            r_divider = c_divider = c_divider<r_divider?c_divider:r_divider; // if (c_divider < r_divider) {r_divider = c_divider; } else { c_divider = r_divider;}
            c_divider*=2;
        }
        else if (is_width)
        {
            r_divider = c_divider;
            r_divider/=2;
        }
        else if (is_height)
        {
            c_divider = r_divider;
            c_divider*=2;
        }
    }
    int w_row = file_header.rows*r_divider; // Number of rows
    int w_col = file_header.columns*c_divider; // Number of columns

    printf("%sFilename:\em %s%s\em  %sSize:\em %s%d/%d\em  %sInterpolation:\em %s%s\em\n", GINGER, GREEN, filename, GINGER, GREEN, w_col, w_row, GINGER, is_inter?GREEN:RED,is_inter?"true":"false");

    struct color colors[w_row][w_col];
    postprocess(c_divider, r_divider, w_row, w_col, colors, file_header, original_colors, is_inter); // Interpolation

    /* Write file in console */

    for (int r=0; r<w_row; r++)
    {
        for (int c=0; c<w_col; c++)
        {
            printf("\e[48;2;%d;%d;%dm ", colors[r][c].red, colors[r][c].green, colors[r][c].blue);
        }
        printf("\n");
    }

    fclose(file);
}


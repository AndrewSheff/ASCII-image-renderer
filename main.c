#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct file_header
{
    char format[2];
    int columns;
    int rows;
    int brightness;
};

struct color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct file_header get_header(FILE *file)
{
    struct file_header file_header;
    fscanf(file, "%s%d%d%d", file_header.format, &file_header.columns, &file_header.rows, &file_header.brightness);
    return file_header;
}

void get_colors(FILE *file, struct file_header file_header, struct color colors[file_header.rows][file_header.columns])
{
    for (int r=0; r<file_header.rows; r++)
    {
        for (int c=0; c<file_header.columns; c++)
        {
            struct color color;
            for (int i=0; i<=2; i++)
            {
                switch (i)
                {
                    case 0:
                    {
                        color.blue = fgetc(file);
                        break;
                    }
                    case 1:
                    {
                        color.red = fgetc(file);
                        break;
                    }
                    case 2:
                    {
                        color.green = fgetc(file);
                        break;
                    }
                }
            }
            colors[r][c] = color;
        }
    }
}

void postprocess(float divider, int w_row, int w_col, struct color colors[w_row][w_col], struct file_header file_header, struct color original_colors[file_header.rows][file_header.columns], int interpolation)
{
    for (int w_r=0; w_r<w_row; w_r++)
    {
        for (int w_c=0; w_c<w_col; w_c++)
        {
            int r = w_r/divider;
            int c = w_c/divider/2;
            struct color i_color = original_colors[r][c];
            if (interpolation)
            {
                int red = 0;
                int green = 0;
                int blue = 0;
                int count = 0;
                for (int i=0; i<file_header.rows/w_row; i++)
                {
                    if (r-i<0)
                    {
                        count++;
                        red = red + original_colors[r-i][c].red;
                        green = green + original_colors[r-i][c].green;
                        blue = blue + original_colors[r-i][c].blue;
                    }
                    if (r+i<=file_header.rows)
                    {
                        count++;
                        red = red + original_colors[r+i][c].red;
                        green = green + original_colors[r+i][c].green;
                        blue = blue + original_colors[r+i][c].blue;
                    }
                    if (c-i<0)
                    {
                        count++;
                        red = red + original_colors[r][c-i].red;
                        green = green + original_colors[r][c-i].green;
                        blue = blue + original_colors[r][c-i].blue;
                    }
                    if (c+i<=file_header.rows)
                    {
                        count++;
                        red = red + original_colors[r][c+i].red;
                        green = green + original_colors[r][c+i].green;
                        blue = blue + original_colors[r][c+i].blue;
                    }
                }
                i_color.red = red / count;
                i_color.green = green / count;
                i_color.blue = blue / count;
            }
            colors[w_r][w_c] = i_color;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argv[1]==NULL)
    {
        printf("First parameter need to be the filename\n");
        return 1;
    }
    if (!strstr(argv[1],".ppm"))
    {
        printf("Unsupported file format, need to be .ppm\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", argv[1]);
        return 1;
    }
    struct file_header file_header = get_header(file);
    struct color original_colors[file_header.rows][file_header.columns];
    get_colors(file, file_header, original_colors);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    float c_divider = (float)(w.ws_col)/file_header.columns;
    float r_divider = ((float)(w.ws_row))/file_header.rows;

    float divider = c_divider<r_divider?c_divider:r_divider;
    int w_row = file_header.rows*divider;
    int w_col = file_header.columns*divider*2;

    struct color colors[w_row][w_col];
    postprocess(divider, w_row, w_col, colors, file_header, original_colors, (argc>2&&strstr(argv[2],"inter")));

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


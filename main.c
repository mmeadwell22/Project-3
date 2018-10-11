#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boot.h"

#define BUFFER_SIZE 1024

void clearTerm()
{
    printf("\033[H\033[J");
}

int getInput(char *input)
{
    if (strstr(input, "quit") == input)
    {
        printf("closing...\n");
        return 1;
    }
    if (strstr(input, "help") == input)
    {
        return 2;
    }
    if (strstr(input, "clear") == input || strstr(input, "clean") == input)
    {
        return 3;
    }
    if (strstr(input, "fmount") == input)
    {
        return 4;
    }
    if (strstr(input, "funmount") == input)
    {
        return 5;
    }

    return 0;
}

int main()
{
    int q = 1;
    size_t buf = BUFFER_SIZE;
    char *input = (char *)malloc(BUFFER_SIZE);

    while (q)
    {
        printf("\nPlease enter your next command\n");
        getline(&input, &buf, stdin);
        char *holder = input;
        while (*holder == ' ')
        {
            holder++;
        }
        char *eol = holder + strlen(holder) - 1;
        if (*eol == '\n')
        {
            *eol = '\0';
        }

        switch (getInput(input))
        {
        case 1:
            q = 0;
            break;
        case 2:
            printf("\nPlease enter one of the following commands.\n help, fmount, funmount, structure, traverse, showsector, showfat, showfile, quit\n");
            break;
        case 3:
            clearTerm();
            break;
        case 4:
            holder += strlen("fmount");
            while (*holder == ' ')
            {
                holder++;
            }
            mount(holder);
            break;
        case 5:
            // holder += strlen("FUNMOUNT");
            // while(*holder == ' ')
            // {
            //     holder++;
            // }
            unmount();
            break;
        case 6:
        default:
            printf("\n%s not a valid command\n", input);
        }
    }

    return 0;
}
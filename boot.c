#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "boot.h"
#include "Fat12Boot.h"
#include "Fat12Entry.h"

char *imgName;
int fileDesc;

void mount(char *file)
{
    imgName = (char *)malloc(strlen(file) + 1);
    strcpy(imgName, file);
    int img = open(file, O_RDWR);
    fileDesc = img;
    if (img < 0)
    {
        printf("\nNo file found with name %s\n", file);
    }
    if (img > 0)
    {
        printf("\n%s mounted successfully\n", file);
    }
}

void unmount()
{
    close(fileDesc);
    printf("\nfinished unmounting %s \n", imgName);
}
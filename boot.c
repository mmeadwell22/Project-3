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

Fat12Boot boot;
Fat12Entry *entry;

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

void traverse (int l) {
    if (l) {
        printf("        *****************************\n");
        printf("        ** FILE ATTRIBUTE NOTATION **\n");
        printf("        **                         **\n");
        printf("        ** R ------ READ ONLY FILE **\n");
        printf("        ** S ------ SYSTEM FILE    **\n");
        printf("        ** H ------ HIDDEN FILE    **\n");
        printf("        ** A ------ ARCHIVE FILE   **\n");
        printf("        *****************************\n");
        printf("\n");

        for (int i=0; i<boot.MAX_ROOT_DIRS; i++) {
            if (entry[i].FILENAME[0] != 0x00 && entry[i].START_CLUSTER != 0) {
                char attr[6] = {'-','-','-','-','-'};
                unsigned char a = entry[i].ATTRIBUTES[0];
                if (a == 0x01)
                    attr[0] = 'R';
                if (a == 0x02)
                    attr[1] = 'H';
                if (a == 0x04)
                    attr[2] = 'S';
                if (a == 0x20)
                    attr[5] = 'A';
                if (a == 0x10) {
                    for (int j = 0; j < 6; j++)
                        attr[j] = '-';
                }

                if (entry[i].ATTRIBUTES[0] == 0x10) {
                    printf("%.6s    %d %d       < DIR >      /%.8s                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILENAME, entry[i].START_CLUSTER);
                    printf("%.6s    %d %d       < DIR >      /%.8s/.                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILENAME, entry[i].START_CLUSTER);
                    printf("%.6s    %d %d       < DIR >      /%.8s/..                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILENAME, 0);
                } else {
                    printf("%.6s    %d %d       %lu      /%.8s.%.3s                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILE_SIZE, entry[i].FILENAME, entry[i].EXT, entry[i].START_CLUSTER);
                }
            }
        }

    } else {
        for (int i=0; i<boot.MAX_ROOT_DIRS; i++) {
            if (entry[i].FILENAME[0] != 0x00 && entry[i].START_CLUSTER != 0) {
                if (entry[i].ATTRIBUTES[0] == 0x10) {
                    printf("/%.8s                       < DIR >\n", entry[i].FILENAME);
                    printf("/%.8s/.                     < DIR >\n", entry[i].FILENAME);
                    printf("/%.8s/..                    < DIR >\n", entry[i].FILENAME);
                    } else {
                        printf("/%.8s.%.3s\n", entry[i].FILENAME, entry[i].EXT);
                    }
            }
        }
    }
}
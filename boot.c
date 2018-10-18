#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "boot.h"
#include "Fat12Boot.h"
#include "Fat12Entry.h"

#define READ_ONLY_MASK 0x01
#define HIDDEN_MASK 0x02
#define SYSTEM_MASK 0x04
#define VOLUME_LABEL_MASK 0x08
#define SUBDIRECTORY_MASK 0x10
#define ARCHIVE_MASK 0x20
#define DAY_MASK 0x001F
#define MONTH_MASK 0x01E0
#define YEAR_MASK 0xFE00
#define HOUR_MASK 0xF800
#define MIN_MASK 0x07E0
#define SEC_MASK 0x01F


char *imgName;
int fileDesc;
struct FatBoot boot;
struct Fat12Entry entry[6656];

unsigned char buf[4608];
unsigned char allEntry[6656];
unsigned short fat[4608];


void readImg()
{
    lseek(fileDesc, 11, SEEK_SET);

    read(fileDesc, &boot.BYTES_PER_SECTOR, 2);

    read(fileDesc, &boot.SECTORS_PER_CLUSTER, 1);

    read(fileDesc, &boot.RESERVED_SECTORS, 2);

    read(fileDesc, &boot.NUMBER_OF_FATS, 1);

    read(fileDesc, &boot.MAX_ROOT_DIRS, 2);

    lseek(fileDesc, 3, SEEK_CUR);

    read(fileDesc, &boot.SECTORS_PER_FAT, 2);

    read(fileDesc, &boot.SECTOR_PER_TRACK, 2);

    read(fileDesc, &boot.NUMBER_OF_HEADS, 2);

    lseek(fileDesc, 11, SEEK_CUR);

    read(fileDesc, &boot.VOLUME_ID, 4);

    read(fileDesc, &boot.VOLUME_LABEL, 11);
    
    lseek(fileDesc, boot.BYTES_PER_SECTOR, SEEK_SET);

    //start of fat table # 1
    read(fileDesc, buf, 4608);

    int i;
    int j = 0;

    //reading the fat table
    for(i = 0; i < sizeof(buf) / 3; i++)
    {

        unsigned char b[3];

        b[0] = buf[j * 3];
        b[1] = buf[j * 3 + 1];
        b[2] = buf[j * 3 + 2];

        fat[i * 2] = (((short)(b[1] & 0x0F)) << 8) | (short)b[0];
        fat[i *2 + 1] = (((short)b[2]) << 4) | (((short)b[1]) >> 4);
        j++;
    }

    //going to root directory
    lseek(fileDesc, ((boot.NUMBER_OF_FATS * boot.SECTORS_PER_FAT) + 1) * boot.BYTES_PER_SECTOR, SEEK_SET);
    
    //reading root directory
    read(fileDesc, allEntry, 6656);

    unsigned char temp[32];
    //unsigned char test[32];2 9728
    //unsigned char subEntries[boot.BYTES_PER_SECTOR];
    int l;
    int m;
    //int p;
    int n = 0;
    int f = 0;
    //int x = 0;
    //int counter = 2; 9728

    for(l = 0; l < boot.MAX_ROOT_DIRS; l++)
    {
        for(m = 0; m < 32; m++)
        {
            temp[m] = allEntry[n];
            n++;
        }
        if(temp[26] == 0 && temp[27] == 0)
        {
            //printf("not a valid entry\n");
        }
        else if(temp[0] == 0 || temp[0] == 0xE5)
        {
            //printf("not a valid entry because deleted\n");
        }
        else
        {
            memcpy(entry[f].FILENAME, temp, 8);
            memcpy(entry[f].EXT, temp + 8, 3);
            memcpy(entry[f].ATTRIBUTES, temp + 11, 1);
            memcpy(entry[f].RESERVED, temp + 12, 2);
            memcpy(&entry[f].CREATION_TIME, temp + 14, 2);
            memcpy(&entry[f].CREATION_DATE, temp + 16, 2);
            memcpy(&entry[f].LAST_ACCESS_DATE, temp + 18, 2);
            memcpy(&entry[f].LAST_WRITE_TIME, temp + 22, 2);
            memcpy(&entry[f].LAST_WRITE_DATE, temp + 24, 2);
            memcpy(&entry[f].START_CLUSTER, temp + 26, 2);
            memcpy(&entry[f].FILE_SIZE, temp + 28, 4);

            f++;
        }
    }
}

void mount(char *file)
{
    imgName = (char *)malloc(strlen(file) + 1);
    strcpy(imgName, file);
    int img = open(file, O_RDONLY);
    fileDesc = img;
    if (img < 0)
    {
        printf("\nNo file found with name %s\n", file);
    }
    if (img > 0)
    {
        printf("\n%s mounted successfully\n", file);
    }
    readImg();
}

void unmount()
{
    if(imgName == NULL)
    {
        printf("No file is mounted.");
        return;
    }
    printf("\nfinished unmounting %s\n", imgName);
    close(fileDesc);
}

void structure()
{
    if(imgName == NULL)
    {
        printf("\nNo file is mounted please try mounting a file.\n");
        return;
    }
    //14
    printf("\nNumber of FAT:                      %d\n", boot.NUMBER_OF_FATS);\
    //30
    printf("Number of sectors used by FAT:      %d\n", boot.SECTORS_PER_FAT);
    //30
    printf("Number of sectors per cluster:      %d\n", boot.SECTORS_PER_CLUSTER);
    //23
    printf("Number of ROOT entries:           %d\n", boot.MAX_ROOT_DIRS);
    //27
    printf("Number of bytes per sector:       %d\n", boot.BYTES_PER_SECTOR);

    printf("--Sector #--\t--Sector Types--");

    printf("\n 0%d -- 0%d             FAT%d", (boot.SECTORS_PER_FAT * 0) + 1, boot.SECTORS_PER_FAT, 1);
    printf("\n %d -- %d             FAT%d", (boot.SECTORS_PER_FAT * 1) + 1, boot.SECTORS_PER_FAT * 2, 2);
    printf("\n %d -- %d   ROOT DIRECTORY\n", boot.SECTORS_PER_FAT * boot.NUMBER_OF_FATS + 1, (boot.MAX_ROOT_DIRS / 16) + boot.SECTORS_PER_FAT * boot.NUMBER_OF_FATS);
}

void showfat()
{
    if(imgName == NULL)
    {
        printf("\nNo file is mounted please try mounting a file.\n");
        return;
    }
    int i;
    printf("         0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F\n");
    printf("         ");
    printf("        ");
    for(i = 2; i < (boot.SECTORS_PER_FAT * boot.BYTES_PER_SECTOR); i++)
    {
        if (i % 16 == 0 || i == 0)
        {
            printf("\n");
            printf("%4X ", i);
        }
        if(fat[i] == 0)
        {
            printf(" FREE ");
        }
        else
        {
            printf("%5X ", fat[i]);
        }
    }
}

void showsector(int secNum)
{
    if(imgName == NULL)
    {
        printf("Please try mounting a image.\n");
        return;
    }
    unsigned char sec;
    lseek(fileDesc, boot.BYTES_PER_SECTOR * secNum, SEEK_SET);

    printf("        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F");

    int i;
    for(i = 0; i < boot.BYTES_PER_SECTOR; i++)
    {
        if(i % 16 == 0 || i == 0)
        {
            printf("\n");
            printf("%4X", i);
        }
        read(fileDesc, &sec, 1);
        printf("%5X", sec);
    }

}

void traverse(int l)
{
    if (imgName == NULL)
    {
        printf("Please try mounting a image.\n");
        return;
    }
    if (l)
    {
        printf("        *****************************\n");
        printf("        ** FILE ATTRIBUTE NOTATION **\n");
        printf("        **                         **\n");
        printf("        ** R ------ READ ONLY FILE **\n");
        printf("        ** S ------ SYSTEM FILE    **\n");
        printf("        ** H ------ HIDDEN FILE    **\n");
        printf("        ** A ------ ARCHIVE FILE   **\n");
        printf("        *****************************\n");
        printf("\n");
        int i;
        int j;
        for (i = 0; i < boot.MAX_ROOT_DIRS; i++)
        {
            if (entry[i].FILENAME != 0x00 && entry[i].START_CLUSTER != 0)
            {
                int day = entry[i].CREATION_DATE & DAY_MASK;
                int month = (entry[i].CREATION_DATE & MONTH_MASK) >> 5;
                int year = (entry[i].CREATION_DATE & YEAR_MASK) >> 9;
                year = year + 1980;
                int sec = entry[i].CREATION_TIME & SEC_MASK;
                int min = (entry[i].CREATION_TIME & MIN_MASK) >> 5;
                int hour = (entry[i].CREATION_TIME & HOUR_MASK) >> 11;
                char attr[6] = {'-', '-', '-', '-', '-'};
                unsigned char a = entry[i].ATTRIBUTES[0];
                if (a == READ_ONLY_MASK)
                    attr[0] = 'R';
                if (a == HIDDEN_MASK)
                    attr[1] = 'H';
                if (a == SYSTEM_MASK)
                    attr[2] = 'S';
                if (a == ARCHIVE_MASK)
                    attr[5] = 'A';
                if (a == SUBDIRECTORY_MASK)
                {
                    for (j = 0; j < 6; j++)
                        attr[j] = '-';
                }

                if (entry[i].ATTRIBUTES[0] == SUBDIRECTORY_MASK)
                {
                    printf("%.6s    %d/%d/%d %d:%d:%d      < DIR >      /%.8s                 %d\n", attr, month, day, year, hour, min, sec, entry[i].FILENAME, entry[i].START_CLUSTER);
                    printf("%.6s    %d/%d/%d %d:%d:%d      < DIR >      /%.8s/.                 %d\n", attr, month, day, year, hour, min, sec, entry[i].FILENAME, entry[i].START_CLUSTER);
                    printf("%.6s    %d/%d/%d %d:%d:%d      < DIR >      /%.8s/..                 %d\n", attr, month, day, year, hour, min, sec, entry[i].FILENAME, 0);
                }
                else
                {
                    printf("%.6s    %d/%d/%d %d:%d:%d       %lu      /%.8s.%.3s                 %d\n", attr, month, day, year, hour, min, sec, entry[i].FILE_SIZE, entry[i].FILENAME, entry[i].EXT, entry[i].START_CLUSTER);
                }
            }
        }
    }
    else
    {
        int i;
        for (i = 0; i < boot.MAX_ROOT_DIRS; i++)
        {
            if (entry[i].FILENAME[0] != 0x00 && entry[i].START_CLUSTER != 0)
            {
                if (entry[i].ATTRIBUTES[0] == SUBDIRECTORY_MASK)
                {
                    printf("/%.8s                       < DIR >\n", entry[i].FILENAME);
                    printf("/%.8s/.                     < DIR >\n", entry[i].FILENAME);
                    printf("/%.8s/..                    < DIR >\n", entry[i].FILENAME);
                }
                else
                {
                    printf("/%.8s.%.3s\n", entry[i].FILENAME, entry[i].EXT);
                }
            }
        }
    }
}

void showfile(char *fileName)
{
    if(imgName == NULL)
    {
        printf("Please try mounting a image.\n");
        return;
    }
    


}

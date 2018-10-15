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
struct FatBoot boot;
//struct Fat12Entry entry[6656];

unsigned char buf[4608];
unsigned char allEntry[6656];
unsigned short fat[4608];
//unsigned short fat2[2880];

//Fat12Entry buf[4096];


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

    //reading fat table
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

    // int k;
    // for(k = 0; k < 100; k++)
    // {
    //     printf("Just testing %X\n", fat[k]);
    // }

    //((2 * 9) + 1) * 512 = 530
    //lseek(fileDesc, ((boot.NUMBER_OF_FATS * boot.SECTORS_PER_FAT) + 1) * boot.BYTES_PER_SECTOR, SEEK_SET);

    //read(fileDesc, allEntry, 6656);

    //int z;

    // for(z = 0; z < boot.MAX_ROOT_DIRS; z++)
    // {
        //int test2 = read(fileDesc, &entry[0].FILENAME, 8);
        //read(fileDesc, &entry[0].EXT, 3);
        //printf("The number of bytes read filename %d\n", test2);
        //printf("The number of bytes read ext %d\n", test3);
        //printf("File name is %.8s\n", entry[0].FILENAME);
        //printf("Extension is %.3s\n", entry[0].EXT);

    //     read(fileDesc, &entry[z].ATTRIBUTES, 1);

    //     read(fileDesc, &entry[z].RESERVED, 2);

    //     read(fileDesc, &entry[z].CREATION_TIME, 2);

    //     read(fileDesc, &entry[z].CREATION_DATE, 2);

    //     read(fileDesc, &entry[z].LAST_ACCESS_DATE, 2);

    //     read(fileDesc, &entry[z].CREATION_TIME, 2);

    //     lseek(fileDesc, 2, SEEK_CUR);

    //     read(fileDesc, &entry[z].LAST_WRITE_TIME, 2);

    //     read(fileDesc, &entry[z].LAST_WRITE_DATE, 2);

    //     read(fileDesc, &entry[z].START_CLUSTER, 2);

    //     read(fileDesc, &entry[z].FILE_SIZE, 4);
        
    // }
    // int y;
    // for (y = 33; y < 65; y++)
    // {
    //     printf("%d byte is %X\n", y, allEntry[y]);
    // }

    //showInfo();

    // int j;

    // for(j = 0; j < 10; j++)
    // {
        //printf("The filename is %.8s.%.3s\n", entry[0].FILENAME, entry[0].EXT);
        
    //}
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
    //int test = ((2*9)+1)*512;
    //printf("value is %d\n", test);
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
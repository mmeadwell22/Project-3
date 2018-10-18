#ifndef FAT12BOOT_H
#define FAT12BOOT_H

struct FatBoot{
    unsigned int BYTES_PER_SECTOR;
    unsigned int SECTORS_PER_CLUSTER;
    unsigned int RESERVED_SECTORS;
    unsigned int NUMBER_OF_FATS;
    unsigned int MAX_ROOT_DIRS;
    unsigned int TOTAL_SECTORS;
    unsigned int SECTORS_PER_FAT;
    unsigned int SECTOR_PER_TRACK;
    unsigned int NUMBER_OF_HEADS;
    unsigned int VOLUME_ID;
    unsigned char VOLUME_LABEL[11];
};

#endif
#ifndef FAT12ENTRY_H
#define FAT12ENTRY_H

struct Fat12Entry {
    unsigned char FILENAME[8];//8
    unsigned char EXT[3];//3
    unsigned char ATTRIBUTES[1];//1
    unsigned char RESERVED[2];//2
    unsigned short CREATION_TIME;//2
    unsigned short CREATION_DATE;//2
    unsigned short LAST_ACCESS_DATE;//2
    //ignore 2
    unsigned short LAST_WRITE_TIME;//2
    unsigned short LAST_WRITE_DATE;//2
    unsigned short START_CLUSTER;//2
    unsigned long FILE_SIZE;//4
};

#endif
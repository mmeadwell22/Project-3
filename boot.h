#ifndef BOOT_H
#define BOOT_H

void readImg();
void mount();
void unmount();
void structure();
void traverse(int l);
void showfat();
void showsector(int l);
void showfile(char *file);

#endif
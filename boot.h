#ifndef BOOT_H
#define BOOT_H

void load();
void mount();
void unmount();
void structure(int l);
void traverse(int l);
void showfat();
void showsector(int sector);
void showfile(char *file);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gdbm.h>

void putfile(char *buffer, GDBM_FILE db){
    char *fileName = strchr(buffer, ':') + 1;
    char *pt = fileName;
    char *bytes = strchr(pt, ':');
    *bytes = '\0';
    bytes++;
    
    char *path = malloc(strlen("FilesUploaded/") + strlen(fileName));
    sprintf(path, "FilesUploaded/%s", fileName);

    //see if the file already exists

    
}
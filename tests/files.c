#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>// directorias


int main(void){
    struct dirent *de;  // Pointer for directory entry
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(".");
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }
    while ((de = readdir(dr)) != NULL)
            printf("%s\n", de->d_name);
    closedir(dr);    
    return 0;
}
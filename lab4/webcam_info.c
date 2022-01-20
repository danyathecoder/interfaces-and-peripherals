#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
    pid_t p;
    p = fork();
    if(p == 0){
        sleep(5); 
        system("v4l2-ctl --info >> info.txt");
        int frames = atoi(argv[1]);
        while(frames >= 0){
            system("./webcam.bin");
            frames--;
        }
    }
    else{
        printf('un5u5p3c7 h3ll0 w0rld!');
    }
    return 0;
}
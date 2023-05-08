#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h> 
#include <sys/wait.h>
#include <pthread.h>
#include <math.h>


int main(int argc) {
    FILE *from_fd, *to_fd;

    if ((from_fd = fopen("video.mp4", "rb")) == NULL) {
        printf("File does not exist\n");
        return -1;
    }

    if ((to_fd = fopen("copy.mp4", "w")) == NULL) {
        printf("Open file failed\n");
        return -1;
    }

    fseek(from_fd, 0, SEEK_END);
    int filesize = ftell(from_fd);
    int file_cnt = filesize / 1024;
    if(filesize % 1024) file_cnt ++;
    // printf("%d\n", file_cnt);
    // Reset file pointer
	fseek(from_fd, 0, SEEK_SET);

    char buffer[1024];
    int cnt = 0;
    
    while(fread(buffer, 1, sizeof(buffer), from_fd)) {
        cnt++;
        // printf("%ld\n", strlen(buffer));
        if(cnt == file_cnt){
            fwrite(buffer, 1, filesize % 1024, to_fd);
        } else {
            fwrite(buffer, 1, sizeof(buffer), to_fd);
        }

        memset(buffer, '\0', sizeof(buffer));
    }

    // printf("%d\n", cnt);

    fseek(to_fd, 0, SEEK_END);
    int to_filesize = ftell(to_fd);
    printf("%d\n", to_filesize);

    fclose(from_fd);
    fclose(to_fd);
    
    return 0;
}
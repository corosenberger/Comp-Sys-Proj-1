#include "Problem1.h"

int main(int argc, char const *argv[]) {
    if(argc < 3) {
        printf("Usage:./myfiles file_in_1 file_in_2 ... [file_out (default:myfile.out)]\n");
        printf("If 3 or more files are given the last will be considered the output\n");
        return 0;
    }
    
    const char* writeFile = (argc > 3) ? argv[--argc]: "myfile.out";
    int fd; if((fd = open(writeFile, O_WRONLY | O_CREAT, 0777)) == -1){
        printf("Could not open output file - %s\n", writeFile);
        return 0;
    }

    int i; for(i = 1; i < argc; i++) {
        CopyFile(fd, argv[i]);
    }

    close(fd);
    printf("Copied found files into %s\n", writeFile);
    return 1;
}

void CopyFile(int fd, const char *file_in) {
    int cfd; if((cfd = open(file_in, O_RDONLY)) == -1) {
        printf("%s - No such file or directory\n", file_in);
        return;
    }
    char buff[2048];

    int bytesRead = 0;
    while((bytesRead = read(cfd, buff, 2048)) >= 1) {
        WriteInFile(fd, buff, bytesRead);
    }
    
    close(cfd);
}

void WriteInFile(int fd, const char *buff, int len) {
    write(fd, buff, len);
}

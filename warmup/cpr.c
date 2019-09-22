#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
/* make sure to use syserror() when a system call fails. see common.h */

void
usage()
{
	fprintf(stderr, "Usage: cpr srcdir dstdir\n");
	exit(1);
}

int isDir(char *path){
    struct stat statPath;
    if(stat(path, &statPath) != 0){
        return 0;
    }
    return S_ISDIR(statPath.st_mode);
}

int copyFile(char *copyFileDir, char *destinationFileDir){
    int copyFile, destFile;
    struct stat st;
    copyFile = open(copyFileDir, O_RDONLY);
    stat(copyFileDir, &st);
    destFile = open(destinationFileDir, O_RDWR|O_CREAT, st.st_mode);
    char buf[1000000];
    int ret = read(copyFile, buf, 1000000);
    write(destFile,buf, ret);
    close(copyFile);
    close(destFile);
    return 1;
}

void makeDir(char *path, char *srcPath){
    struct stat temp = {0};
    if(stat(path, &temp) == -1){
        mkdir(path, 0700);
    }
    else{
      fprintf (stderr, "mkdir: %s: File exists", path);
      exit (EXIT_FAILURE);
    }
}

void assignPerms(char *path, char *srcPath){
    struct stat st;
    stat(srcPath, &st);
    printf(srcPath);
    printf("\n");
    printf(path);
    printf("\n");
    printf("%d", st.st_mode);
    printf("\n");
    chmod(path, st.st_mode);
}

void folderIteratorAssignPerms(char *sourceDir, char *desintationDir){
    struct dirent *path;
    DIR *openedPath = opendir(sourceDir);
    while((path = readdir(openedPath)) != NULL){
        if(strcmp(path->d_name, ".") == 0 || strcmp(path->d_name, "..") == 0){
            continue;
        }
        char fullReadPath[256];
        char fullDestPath[256];
        snprintf(fullReadPath, sizeof fullReadPath, "%s%s%s", sourceDir, "/", path->d_name);
        snprintf(fullDestPath, sizeof fullDestPath, "%s%s%s", desintationDir, "/", path->d_name);
        if(isDir(fullReadPath) == 1){
            assignPerms(fullDestPath, fullReadPath);
            folderIteratorAssignPerms(fullReadPath, fullDestPath);
        }
    }
}


void folderIterator(char *sourceDir, char *desintationDir){
    makeDir(desintationDir, sourceDir);
    struct dirent *path;
    DIR *openedPath = opendir(sourceDir);
    if (!openedPath){
        printf("no files\n");
        closedir(openedPath);
        return;
    }

    while((path = readdir(openedPath)) != NULL){
        if(strcmp(path->d_name, ".") == 0 || strcmp(path->d_name, "..") == 0){
            continue;
        }
        char fullReadPath[256];
        char fullDestPath[256];
        snprintf(fullReadPath, sizeof fullReadPath, "%s%s%s", sourceDir, "/", path->d_name);
        snprintf(fullDestPath, sizeof fullDestPath, "%s%s%s", desintationDir, "/", path->d_name);
        if(isDir(fullReadPath) == 1){
            folderIterator(fullReadPath, fullDestPath);
        }
        else{
            copyFile(fullReadPath, fullDestPath);
        }

    }
    closedir(openedPath);
}


int
main(int argc, char *argv[])
{
	if (argc != 3) {
		usage();
	}
        
        char* srcPath = argv[1];
        char* destPath = argv[2];
        folderIterator(srcPath, destPath);
        assignPerms(destPath, srcPath);
        folderIteratorAssignPerms(srcPath, destPath);
	return 1;
}

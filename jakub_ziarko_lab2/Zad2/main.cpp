#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <dirent.h>
#include <cstring>

void print_files_last_modification_date(const char *file_path)
{
    struct stat attrib;
    stat(file_path, &attrib);
    char date[10];
    strftime(date, 10, "%d-%m-%y", gmtime(&(attrib.st_ctime)));
    printf("%s", date);
    date[0] = 0;
}

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {

    if (tflag == FTW_F){
        char sth[PATH_MAX+1];
        char * actual_path;
        actual_path = realpath(fpath, sth);
        print_files_last_modification_date(fpath);
        printf("        -");
        if(sb->st_mode & S_IRUSR) printf("r");
            else printf("-");
        if(sb->st_mode & S_IWUSR) printf("w");
            else printf("-");
        if(sb->st_mode & S_IXUSR) printf("x");
            else printf("-");
        if(sb->st_mode & S_IRGRP) printf("r");
            else printf("-");
        if(sb->st_mode & S_IWGRP) printf("w");
            else printf("-");
        if(sb->st_mode & S_IXGRP) printf("x");
            else printf("-");
        if(sb->st_mode & S_IROTH) printf("r");
            else printf("-");
        if(sb->st_mode & S_IWOTH) printf("w");
            else printf("-");
        if(sb->st_mode & S_IXOTH) printf("x");
            else printf("-");
        printf("       %i   %8i B    %s\n", ftwbuf->level, static_cast <int>(sb->st_size), actual_path);
    }
    return 0;
}

void explore_folders(char * path, int level) {
    struct dirent * dirent1;
    char * actual_path;
    char filename[FILENAME_MAX];
    char sth[PATH_MAX+1];
    DIR * dir = opendir(path);
    struct stat sb;

    if (lstat(path, &sb) == -1) printf("Wrong path");
    int length = strlen(path);
    strcpy(filename, path);  //
    if(filename[length-1] != '/'){
        filename[length] = '/';
        length++;
    }
    while ((dirent1 = readdir(dir)) != NULL){
        if (strcmp(dirent1->d_name, "..") != 0 && strcmp(dirent1->d_name, ".") != 0){

            strncpy(filename + length, dirent1->d_name, FILENAME_MAX - length);
            lstat(filename, &sb);
            if (S_ISDIR(sb.st_mode)){
                explore_folders(filename, level + 1);
            }
            else if (S_ISREG(sb.st_mode) == 1){
                FILE * f = fopen(filename, "r");
                fseek(f, 0, SEEK_END);
                long file_size = ftell(f);
                actual_path = realpath(filename, sth);

                print_files_last_modification_date(actual_path);
                printf("        -");
                if(sb.st_mode & S_IRUSR) printf("r");
                else printf("-");
                if(sb.st_mode & S_IWUSR) printf("w");
                else printf("-");
                if(sb.st_mode & S_IXUSR) printf("x");
                else printf("-");
                if(sb.st_mode & S_IRGRP) printf("r");
                else printf("-");
                if(sb.st_mode & S_IWGRP) printf("w");
                else printf("-");
                if(sb.st_mode & S_IXGRP) printf("x");
                else printf("-");
                if(sb.st_mode & S_IROTH) printf("r");
                else printf("-");
                if(sb.st_mode & S_IWOTH) printf("w");
                else printf("-");
                if(sb.st_mode & S_IXOTH) printf("x");
                else printf("-");
                printf("       %i   %8i B    %s\n", level, static_cast <int>(sb.st_size), actual_path);
                fclose(f);
            }
        }
    }
    closedir(dir); //zamykamy
}

int main(int argc, char *argv[]) {

    int option;
    if (argc != 3){
        printf("First argument should be path, second </=/>.\n");
        exit(EXIT_SUCCESS);
    }

    printf("Which option you want to try?\n1 -- opendir, readdir, stat    \n2 -- nftw.\nOption: ");
    scanf("%i", &option);

    printf("\n\nmodified        permissions     lvl        size    path\n");
    if (option == 2) {
        int flags = FTW_PHYS;
        if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    if (option == 1){
        explore_folders(const_cast<char *>("../../catalogues"), 1);
        exit(EXIT_SUCCESS);
    }
}
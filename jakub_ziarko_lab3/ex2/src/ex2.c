#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ftw.h>
#include <zconf.h>
#include <fcntl.h>
#include <sys/wait.h>


void empty_string(char * string, int size){
    int i;
    for (i = 0; i < size; i++){
        string[i] = 0;
    }
}
int skip_white(char * instructions, int start){
    int i = start;
    while ((instructions [i] == ' ' || instructions[i] == '\t') && instructions[i] != '0'){
        char c = instructions[i];
        i++;
    }
    if (instructions[i-1] == '0')
        return -1;
    return i;
}
int parse_argument(char * instructions, int start, char * arg, int length){
    int k = 0, i = start;
    while(instructions[i] != ' ' && instructions[i] != '\t' && instructions[i] != '0' && i < length){
        arg[k] = instructions[i];
        i++;
        k++;
    }
    arg[k] = '\0';
    if (instructions[i-1] == '0')
        return -1;
    return i;
}
int look_for_program(char * name, char * path){
    struct dirent * dirent1;
    DIR * dir = opendir(path);
    struct stat sb;

    if (lstat(path, &sb) == -1) return -1;

    while((dirent1 = readdir(dir)) != NULL){
        if (strcmp(dirent1->d_name, name) == 0){
            printf("NAME: %s  %s\n", dirent1->d_name, name);
            return 1;
        }
    }
    return -1;
}
int create_open_file_sys(char *path){
    return open(path, O_RDWR | O_CREAT, 0777);
}
int parse_line_of_file(int file_desc, char * buffor, int buffer_size, char * instructions_buf){
    empty_string(instructions_buf, 1000);
    if (file_desc < 0) return file_desc;
    int r = (int) read(file_desc, buffor, (size_t) buffer_size);
    if ( r == 0 ) return -1;
    while (r > 0 && strcmp(buffor, "\n")){
        strcat(instructions_buf, buffor);
        r = (int) read(file_desc, buffor, (size_t) buffer_size);
    }
    return 1;
}
void run_program(int file_desc, char * buffor, int buffer_size, char * instructions_buf, char * arg, char ** array, char * path_to_program, int env) {
    pid_t child_pid;
    int i, j = 1, index, status = 0, flag = 0;
    while (j != -1) {
        if (env == 1){
            strcpy(path_to_program, getenv("PATH"));
        }
        else strcpy(path_to_program, "/bin/");
        j = parse_line_of_file(file_desc, buffor, buffer_size, instructions_buf);
        int l = (int) strlen(instructions_buf);
        index = 0;
        i = 0;
        while (i < l && j != -1) {
            i = skip_white(instructions_buf, i);
            empty_string(arg, 100);
            i = parse_argument(instructions_buf, i, arg, l);
            strcpy(array[index], arg);
            index++;
        }
        if ( j != -1) {
            array[index] = (char *) NULL;
            if (env != 1)
                strcat(path_to_program, array[0]);
            child_pid = fork();
            if (child_pid == 0) {
                flag = execl(path_to_program, array[0], array[1], array[2], array[3], array[4]);
            }
            else if (child_pid > 0) {
                wait(&status);
                if (flag == -1) {
                    printf("Something went wrong.\n");
                    return;
                }
            }
        }
    }
}
int main(int argc, char *argv[]) {
    int i, array_size = 100, file_desc;
    const int buffor_size = 1;
    const int instructions_size = 1000;
    const int path_to_program_size = 100;
    const int block_size = 100;
    const int arg_size = 100;
    const int file_name_size = 100;
    int env_variable = 0;
    char * path_to_program = calloc((size_t) path_to_program_size, sizeof(char));
    char * arg = calloc((size_t) arg_size, sizeof(char));
    char * buffor = calloc((size_t) buffor_size, sizeof(char));
    char * instructions_buf = calloc((size_t) instructions_size, sizeof(char));
    char * file_name = calloc((size_t) file_name_size, sizeof(char));
    char ** array;
    array = (char**) calloc((size_t) array_size, sizeof(char*));
    for (i = 0; i < array_size; i++){
        array[i] = calloc((size_t) block_size, sizeof(char));
    }


    printf("%s", "W pierwszym argumencie podaj ścieżkę do pliku z którego program będzie czytać komendy,\n "
    "w drugim czy chcesz użyć zmiennej środowiskowej podając 1 jeśli tak, 0 jeśli nie.\n\n");

    file_name = argv[1];
    env_variable = atoi(argv[2]);

    file_desc = create_open_file_sys(file_name);
    run_program(file_desc, buffor, buffor_size, instructions_buf, arg, array, path_to_program, env_variable);

    return 0;
}
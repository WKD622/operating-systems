//PRZYKLADOWE WYWOLANIA W PLIKU raport.txt

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>

//STRUCTURES
struct timeMeasurment{
    float kernelTime;
    float userTime;
};

//TIME
float sumMicroSecAndSec(int sec, int microSec){
    return (float) microSec / 1000000 + (float) sec;
}
void measureTime(struct timeMeasurment * mes, char * message, FILE *raport){
    struct timeval user;
    struct timeval kernel;
    struct rusage rusage1;
    float secKernel, secUser;

    getrusage(RUSAGE_SELF, &rusage1);
    user = rusage1.ru_utime;
    kernel = rusage1.ru_stime;
    secKernel = sumMicroSecAndSec(kernel.tv_sec, kernel.tv_usec);
    secUser = sumMicroSecAndSec(user.tv_sec, user.tv_usec);
    printf("%s kernel: %fs   |   user: %fs\n\n", message, secKernel - mes->kernelTime, secUser - mes->userTime);
    fprintf(raport, "%s kernel: %fs   |   user: %fs\n\n", message, secKernel - mes->kernelTime, secUser - mes->userTime);
    mes->kernelTime = secKernel;
    mes->userTime = secUser;
}

//STRING
int string_lenght(char *string){
    int lenght = 0;
    while (string[lenght] != 0)
        lenght++;
    return lenght;
}
char *concat_strings(char *string_1, char *string_2){
    int length_1 = string_lenght(string_1);
    int length_2 = string_lenght(string_2);
    char * concat = calloc(length_1 + length_2, sizeof(char));
    strcpy(concat, string_1);
    strcat(concat, string_2);
    return concat;
}
int find_eol_position_in_string(char *string, int string_length){
    int count = 0;
    char c = string[count];
    while (c != '\n' && count < string_length){
        c = string[count];
        count++;
    }
    if (count == 0) count++;
    if (string[count - 1] != '\n') return -1;
        else return count;
}
int count_eol_in_string(char *string, int string_length){
    int i = 0, count = 0;
    while (i < string_length){
        if (string[i] == '\n') count++;
        i++;
    }
    return count;
}
void copy_block_of_chars(char *from, char *to, int length){
    int i;
    for (i = 0; i < length ; i++){
        to[i] = from[i];
    }
}

//FILES SYSTEM
int create_open_file_sys(char *path, char *file_name){
    const char * file_path = concat_strings(path, file_name);
    return open(file_path, O_RDWR | O_CREAT, 0777);
}
int create_open_clean_file_sys(char *path, char *file_name){
    const char * file_path = concat_strings(path, file_name);
    return open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
}
int record_length_sys(char *path, char *file_name){
    int length = 0, buffer_size = 40, position = -1;
    char * buffer = calloc((size_t) buffer_size, sizeof(char));
    char c = 'a';
    int file_desc = create_open_file_sys(path, file_name);
    if (file_desc < 0) return file_desc;

    while (position == -1){
        int flag = (int) read(file_desc, buffer, (size_t) buffer_size);
        position = find_eol_position_in_string(buffer, flag);
        if (position == -1) length += buffer_size;
            else length += position;
    }
    free(buffer);
    close(file_desc);
    return length-1;
}
int count_records_sys(char *path, char *file_name){
    int count = 0, buffer_size = 40;
    int file_desc = create_open_file_sys(path, file_name);
    if (file_desc < 0) return file_desc;
    char *buffer = calloc((size_t) buffer_size, sizeof(char));
    int buffer_flag = (int) read(file_desc, buffer, (size_t) buffer_size);
    count += count_eol_in_string(buffer, buffer_flag);
    while (buffer_flag > 0){
        buffer_flag = (int) read(file_desc, buffer, (size_t) buffer_size);
        count += count_eol_in_string(buffer, buffer_flag);
    }
    free(buffer);
    close(file_desc);
    return count;
}
int calculate_offset(int record_number, int record_length){
    return record_number * (record_length + 1);
}
char first_char_of_record_sys(int file_desc, int record_number, int record_length, char *buffor){
    int offset = calculate_offset(record_number, record_length);
    lseek(file_desc, offset, SEEK_SET);
    read(file_desc, buffor, (size_t) record_length + 1);
    lseek(file_desc, 0, SEEK_SET);
    return buffor[0];
}
void print_file_sys(int file_desc, int record_length){
    printf("\n\n\n");
    char * buffer = calloc((size_t) record_length, sizeof(char));
    int fileBuffer = (int) read(file_desc, buffer, (size_t) record_length);
    printf("%s", buffer);
    while (fileBuffer > 0){
        fileBuffer = (int) read(file_desc, buffer, (size_t) record_length);
        printf("%s", buffer);
    }
    printf("\n\n\n");
}
void move_record_sys(int file_desc, int sorted_records, int record_length, char cw, char *to_place){
    int i = 0;
    int offset = calculate_offset(i, record_length);
    char * buffor = calloc((size_t) record_length + 1, sizeof(char));
    char cp = first_char_of_record_sys(file_desc, i, record_length, buffor);
    while (i <= sorted_records && cw > cp){
        i++;
        cp = first_char_of_record_sys(file_desc, i, record_length, buffor);
        offset += record_length + 1;
    }
    if (i != sorted_records){
        lseek(file_desc, offset, SEEK_SET);
        write(file_desc, to_place, (size_t) record_length + 1);
        while (i < sorted_records){
            i++;
            copy_block_of_chars(buffor, to_place, record_length + 1);
            offset += record_length + 1;
            lseek(file_desc, offset, SEEK_SET);
            read(file_desc, buffor, (size_t) record_length + 1);
            lseek(file_desc, offset, SEEK_SET);
            write(file_desc, to_place, (size_t) record_length + 1);
        }
    }
    free(buffor);
    lseek(file_desc, 0, SEEK_SET);
}

//FILES LIBRARY
FILE *create_open_file_lib(char *path, char *file_name){
    char *file_path = concat_strings(path, file_name);
    return fopen(file_path, "r+");
}
FILE *create_open_clean_file_lib(char *path, char *file_name){
    char *file_path = concat_strings(path, file_name);
    return fopen(file_path, "w+");
}
int record_length_lib(char *path, char *file_name){
    int length = 0, buffer_size = 40, position = -1, flag = 1;
    char * buffer = calloc((size_t) buffer_size, sizeof(char));
    char c = 'a';
    FILE * f = create_open_file_lib(path, file_name);
    if (f == NULL ) return -1;

    while (position == -1 && flag != 0){
        flag = (int) fread (buffer, sizeof(char), buffer_size, f);
        position = find_eol_position_in_string(buffer, flag);
        if (position == -1) length += buffer_size;
        else length += position;
    }
    free(buffer);
    fclose(f);
    return length-1;
}
int count_records_lib(char *path, char *file_name){
    int count = 0, buffer_size = 40;
    FILE * f = create_open_file_lib(path, file_name);
    if (f == NULL)
        return -1;
    char *buffer = calloc((size_t) buffer_size, sizeof(char));
    int buffer_flag = (int) fread(buffer, sizeof(char), buffer_size, f);
    count += count_eol_in_string(buffer, buffer_flag);
    while (buffer_flag > 0){
        buffer_flag = (int) fread(buffer, sizeof(char), buffer_size, f);
        count += count_eol_in_string(buffer, buffer_flag);
    }
    free(buffer);
    fclose(f);
    return count;
}
char first_char_of_record_lib(FILE *f, int record_number, int record_length, char *buffor){
    int offset = calculate_offset(record_number, record_length);
    fseek(f, offset, 0);
    fread(buffor, sizeof(char), (size_t) (record_length + 1), f);
    fseek(f, offset, 0);
    return buffor[0];
}
void move_record_lib(FILE *f, int sorted_records, int record_length, char cw, char *to_place){
    int i = 0;
    int offset = calculate_offset(i, record_length);
    char * buffor = calloc((size_t) record_length + 1, sizeof(char));
    char cp = first_char_of_record_lib(f, i, record_length, buffor);
    while (i <= sorted_records && cw > cp){
        i++;
        cp = first_char_of_record_lib(f, i, record_length, buffor);
        offset += record_length + 1;
    }
    if (i != sorted_records){
        fseek(f, offset, 0);
        fwrite(to_place, sizeof(char), (size_t) (record_length + 1), f);
        while (i < sorted_records){
            i++;
            copy_block_of_chars(buffor, to_place, record_length + 1);
            offset += record_length + 1;
            fseek(f, offset, 0);
            fread(buffor, sizeof(char), (size_t) (record_length + 1), f);
            fseek(f, offset, 0);
            fwrite(to_place, sizeof(char), (size_t) (record_length + 1), f);
        }
    }
    free(buffor);
    fseek(f, 0, SEEK_SET);
}

//GENERATE
char random_char_generator(){
    const char * alphabet = "abcdefghijklmnopqrstuvwxyz";
    return alphabet[rand()%26];
}
char *random_record_of_chars(int length, char *record){
    int i;
    record = calloc(length, sizeof(char));
    for (i = 0; i < length-1; i++){
        record[i] = random_char_generator();
    }
    record[length-1] = '\n';
    return record;
}
int generate_random_file_sys(int number_of_records, int length_of_record, char *path, char *file_name){
    char * record;
    int i, file_desc = create_open_clean_file_sys(path, file_name);
    if (file_desc < 0)
        return file_desc;
    for (i = 0; i < number_of_records; i++)
        write(file_desc, random_record_of_chars(length_of_record + 1, record), length_of_record+1);
    close(file_desc);
    return 1;
}
int generate_random_file_lib(int number_of_records, int length_of_record, char *path, char *file_name){
    char * record;
    int i;
    FILE * f = create_open_clean_file_lib(path, file_name);
    if (f == NULL)
        return -1;
    for (i = 0; i < number_of_records; i++)
        fwrite(random_record_of_chars(length_of_record + 1, record), sizeof(char), length_of_record + 1, f);
    fclose(f);
    return 1;
}

//COPY
int copy_sys(char *from_path, char *from_name, char *to_path, char *to_name, int buffer_size, int number_of_records_to_copy){
    int i = 1;
    int to_file_desc, from_file_desc, buffer_flag;
    char * buffer = calloc( (size_t) buffer_size, sizeof(char));
    from_file_desc = create_open_file_sys(from_path, from_name);
    if (from_file_desc < 0) return from_file_desc;
    to_file_desc = create_open_clean_file_sys(to_path, to_name);
    if (to_file_desc < 0) return to_file_desc;

    buffer_flag = (int) read(from_file_desc, buffer, (size_t) buffer_size);
    write(to_file_desc, buffer, (size_t) buffer_flag);
    while (buffer_flag != 0 && i <= number_of_records_to_copy){
        buffer_flag = (int) read(from_file_desc, buffer, (size_t) buffer_size);
        write(to_file_desc, buffer, (size_t) buffer_flag);
        i++;
    }
    close(to_file_desc);
    close(from_file_desc);
    free(buffer);
    return 1;
}
int copy_lib(char *from_path, char *from_name, char *to_path, char *to_name, int buffer_size, int number_of_records_to_copy){
    int buffer_flag, i = 1;
    char * buffer = calloc( (size_t) buffer_size, sizeof(char));
    FILE * from = create_open_file_lib(from_path, from_name);
    if (from == NULL) return -1;
    FILE * to = create_open_clean_file_lib(to_path, to_name);
    if (to == NULL) return -1;

    buffer_flag = (int) fread(buffer, sizeof(char), buffer_size, from);
    fwrite(buffer, sizeof(char), (size_t) buffer_flag, to);
    while (buffer_flag != 0 && i <= number_of_records_to_copy){
        buffer_flag = (int) fread(buffer, sizeof(char), buffer_size, from);
        fwrite(buffer, sizeof(char), (size_t) buffer_flag, to);
        i++;
    }
    fclose(from);
    fclose(to);
    free(buffer);
    return 1;
}

//SORT
int sort_sys(char *path, char *file_name){
    int file_desc = create_open_file_sys(path, file_name);
    if (file_desc < 0) return file_desc;
    int record_len = record_length_sys(path, file_name);
    int record_numbers = count_records_sys(path, file_name);
    char * buffor = calloc( (size_t) record_len + 1, sizeof(char));
    int sorted_records = 1;

    while (sorted_records < record_numbers){
        char c = first_char_of_record_sys(file_desc, sorted_records, record_len, buffor);
        move_record_sys(file_desc, sorted_records, record_len, c, buffor);
        sorted_records++;
    }
    free(buffor);
    close(file_desc);
    return 1;
}
int sort_lib(char *path, char *file_name){
    FILE * f = create_open_file_lib(path, file_name);
    if (f == NULL) return -1;
    int record_len = record_length_lib(path, file_name);
    int record_numbers = count_records_lib(path, file_name);
    char * buffor = calloc( (size_t) record_len + 1, sizeof(char));
    int sorted_records = 1;

    while (sorted_records < record_numbers){
        char c = first_char_of_record_lib(f, sorted_records, record_len, buffor);
        move_record_lib(f, sorted_records, record_len, c, buffor);
        sorted_records++;
    }
    free(buffor);
    fclose(f);
    return 1;
}

//MAIN
int main(int argc, char *argv[]) {
    FILE *raport = fopen("raport.txt", "w");
    struct timeMeasurment mes;
    struct timeval user;
    struct timeval kernel;
    struct rusage rusage1;
    float startUserTime, startKernelTime;
    int number_of_records, length_of_record, buffer_size;
    char *path, *file_name;
    char *from_path, *from_file_name, *to_file_name, *to_path;
    srand(time(NULL));

    getrusage(RUSAGE_SELF, &rusage1);
    user = rusage1.ru_utime;
    kernel = rusage1.ru_stime;
    mes.kernelTime = sumMicroSecAndSec(kernel.tv_sec, kernel.tv_usec);
    mes.userTime = sumMicroSecAndSec(user.tv_sec, user.tv_usec);
    startKernelTime = mes.kernelTime;
    startUserTime = mes.userTime;

    if (strcmp(argv[1], "generate") == 0 ){
        path = argv[2];
        file_name = argv[3];
        number_of_records = atoi(argv[4]);
        length_of_record = atoi(argv[5]);

        if (strcmp(argv[6], "sys") == 0){
            generate_random_file_sys(number_of_records, length_of_record, path, file_name);
        }
        else if (strcmp(argv[6], "lib") == 0 ){
            generate_random_file_lib(number_of_records, length_of_record, path, file_name);
        }
    }

    else if (strcmp(argv[1], "sort") == 0){
        path = argv[2];
        file_name = argv[3];

        if (strcmp(argv[4], "sys") == 0 ){
            sort_sys(path, file_name);
        }
        else if (strcmp(argv[4], "lib") == 0 ){
            sort_lib(path, file_name);
        }
    }

    else if (strcmp(argv[1], "copy") == 0){
        from_path = argv[2];
        from_file_name = argv[3];
        to_path = argv[4];
        to_file_name = argv[5];
        number_of_records = atoi(argv[6]);
        buffer_size = atoi(argv[7]);

        if (strcmp(argv[8], "sys") == 0 ){
            copy_sys(from_path, from_file_name, to_path, to_file_name, buffer_size, number_of_records);
        }
        else if (strcmp(argv[8], "lib") == 0 ){
            copy_lib(from_path, from_file_name, to_path, to_file_name, buffer_size, number_of_records);
        }
    }

    measureTime(&mes, "AFTER ALL OPERATIONS", raport);
    return 0;
}

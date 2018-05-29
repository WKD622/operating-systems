#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <ftw.h>
#include <zconf.h>

void print_files_last_modification_date(const char *file_path) {
	struct stat attrib;
	stat(file_path, &attrib);
	char date[10];
	strftime(date, 10, "%d-%m-%y", gmtime(&(attrib.st_ctime)));
	printf("%s", date);
	date[0] = 0;
}

void explore_folders(char * path, int level) {
	int count = 0;
	pid_t pid;
	struct dirent * dirent1;
	char * actual_path;
	char filename[FILENAME_MAX];
	char sth[PATH_MAX + 1];
	DIR * dir = opendir(path);
	struct stat sb;

	if (lstat(path, &sb) == -1)
		printf("Wrong path");
	int length = (int) strlen(path);
	strcpy(filename, path);
	if (filename[length - 1] != '/') {
		filename[length] = '/';
		length++;
	}
	pid = vfork();
	while ((dirent1 = readdir(dir)) != NULL) {
		if (strcmp(dirent1->d_name, "..") != 0
				&& strcmp(dirent1->d_name, ".") != 0) {
			strncpy(filename + length, dirent1->d_name,
					(FILENAME_MAX - length));
			lstat(filename, &sb);
			if (S_ISDIR(sb.st_mode)) {
				printf("%d\n", getpid());
				explore_folders(filename, level + 1);
			} else if (S_ISREG(sb.st_mode) == 1) {
				count++;
				FILE * f = fopen(filename, "r");
				fseek(f, 0, SEEK_END);
				long file_size = ftell(f);
				actual_path = realpath(filename, sth);

				print_files_last_modification_date(actual_path);
				printf("        -");
				if (sb.st_mode & S_IRUSR)
					printf("r");
				else
					printf("-");
				if (sb.st_mode & S_IWUSR)
					printf("w");
				else
					printf("-");
				if (sb.st_mode & S_IXUSR)
					printf("x");
				else
					printf("-");
				if (sb.st_mode & S_IRGRP)
					printf("r");
				else
					printf("-");
				if (sb.st_mode & S_IWGRP)
					printf("w");
				else
					printf("-");
				if (sb.st_mode & S_IXGRP)
					printf("x");
				else
					printf("-");
				if (sb.st_mode & S_IROTH)
					printf("r");
				else
					printf("-");
				if (sb.st_mode & S_IWOTH)
					printf("w");
				else
					printf("-");
				if (sb.st_mode & S_IXOTH)
					printf("x");
				else
					printf("-");
				printf("        %8i B    %s\n", (int) sb.st_size, actual_path);
				fclose(f);
			}
		}
	}
	exit(pid);
	closedir(dir);
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("First argument should be path, second </=/>.\n");
		exit(EXIT_SUCCESS);
	}

	printf(
			"\n\npid     modified        permissions          size       path\n");

	explore_folders(argv[1], 1);
	exit(EXIT_SUCCESS);
}

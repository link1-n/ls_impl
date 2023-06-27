#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

void sex(const char* dir_name, int list_all) {
	struct dirent *d;
	DIR *dir = opendir(dir_name);

	if (!dir) {
		if (errno = ENOENT) {
			perror("Directory does not exist.");
		}
		else {
			perror("Unable to read directory.");
		}
		exit(EXIT_FAILURE);
	}

	while (( d = readdir(dir)) != NULL ) {
		if (!list_all && d->d_name[0] == '.') {
			continue;
		}
		printf("%s ", d->d_name);
	}
}

int main() {
	//printf("hi");

	sex(".", 1);

	return 1;
}

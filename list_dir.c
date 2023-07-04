#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct file_data {
	const char* file_name;
	long unsigned int num_links;
	unsigned int user_id;
	unsigned int group_id;
	long long file_size;
	time_t mod_time;

	const char* user_name;
	const char* group_name;
	const char* mod_time_str;
};

void get_user_name(struct file_data* obj) {
	struct passwd *pwd;
	if ((pwd = getpwuid(obj->user_id)) != NULL) {
		obj->user_name = pwd->pw_name;
	}
	else {
		obj->user_name = NULL;
	}
}

void get_group_name(struct file_data* obj) {
	struct group* grp;
	if ((grp = getgrgid(obj->group_id)) != NULL) {
		obj->group_name = grp->gr_name;
	}
	else {
		obj->group_name = NULL;
	}
}

void get_datetime(struct file_data* obj) {
	struct tm ts;
	char res[80];

	ts = *localtime(&obj->mod_time);
	strftime(res, sizeof(res), "%Y-%m-%d %H:%M:%S", &ts);

	obj->mod_time_str = res;
}

void print_info(const struct file_data* obj) {
	printf("%-10ld %-10s %-10s %-10lld %-20s %-80s\n",
			obj->num_links,
			obj->user_name,
			obj->group_name,
			obj->file_size,
			obj->mod_time_str,
			obj->file_name);
}

void show(const char* dir_name, int list_all) {
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
		struct stat buf;
		char path[PATH_MAX];
		snprintf(path, PATH_MAX, "%s/%s", dir_name, d->d_name);
		if (stat(path, &buf)) {
			printf("stat error");
			continue;
		}
		if (!list_all && d->d_name[0] == '.') {
			continue;
		}
		struct file_data data_obj;
		data_obj.file_name = d->d_name;
		data_obj.num_links = buf.st_nlink;
		data_obj.user_id = buf.st_uid;
		data_obj.group_id = buf.st_gid;
		data_obj.file_size = buf.st_size;
		data_obj.mod_time = buf.st_mtime;

		get_user_name(&data_obj);
		get_group_name(&data_obj);
		get_datetime(&data_obj);

		print_info(&data_obj);
	}
}

int main(int argc, char* argv[]) {
	show(".", 1);

	return 1;
}

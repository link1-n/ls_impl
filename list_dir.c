#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define MAX_FILES 100

struct file_data {
	char* file_name;
	long unsigned int num_links;
	unsigned int user_id;
	unsigned int group_id;
	long long file_size;
	time_t mod_time;

	char* user_name;
	char* group_name;
	char* mod_time_str;
};

int cmp(const void* a, const void* b) {
	const struct file_data* file_a = *(struct file_data**)a; 
	const struct file_data* file_b = *(struct file_data**)b; 

	//return strcasecmp(file_a->file_name, file_b->file_name);
	return strcmp(file_a->file_name, file_b->file_name);
}

void get_user_name(struct file_data* obj) {
	struct passwd *pwd;
	if ((pwd = getpwuid(obj->user_id)) != NULL) {
		obj->user_name = strdup(pwd->pw_name);
	}
	else {
		obj->user_name = NULL;
	}
}

void get_group_name(struct file_data* obj) {
	struct group* grp;
	if ((grp = getgrgid(obj->group_id)) != NULL) {
		obj->group_name = strdup(grp->gr_name);
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

	obj->mod_time_str = strdup(res);
}

void print_info(const struct file_data* obj, int max_link_len,
		int max_user_name_len,
		int max_group_name_len,
		int max_size_len
		) {
	printf("%*ld ", max_link_len, obj->num_links);
	printf("%*s ", max_user_name_len, obj->user_name);
	printf("%*s ", max_group_name_len, obj->group_name);
	printf("%*lld ", max_size_len, obj->file_size);
	printf("%-18s ", obj->mod_time_str);
	printf("%s\n", obj->file_name);
}

void fill_data(struct file_data* obj, struct stat stat_obj
		,struct dirent* d_obj) {
	obj->file_name = d_obj->d_name;
	obj->num_links = stat_obj.st_nlink;
	obj->user_id = stat_obj.st_uid;
	obj->group_id = stat_obj.st_gid;
	obj->file_size = stat_obj.st_size;
	obj->mod_time = stat_obj.st_mtime;

	get_user_name(obj);
	get_group_name(obj);
	get_datetime(obj);
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

	struct file_data* file_list[MAX_FILES];
	int file_itr = 0;
	size_t max_user_name_len = 0;
	size_t max_group_name_len = 0;
	size_t max_size_len = 0;
	size_t max_link_len = 0;

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
		struct file_data* data_obj = malloc(sizeof(struct file_data));

		fill_data(data_obj, buf, d);

		size_t link_len = floor(log10(abs(data_obj->num_links))) + 1;
		size_t size_len = floor(log10(abs(data_obj->file_size))) + 1;
		size_t group_len = strlen(data_obj->group_name);
		size_t user_len = strlen(data_obj->user_name);

		max_group_name_len = fmax(group_len, max_group_name_len);
		max_user_name_len = fmax(user_len, max_user_name_len);
		max_link_len = fmax(link_len, max_link_len);
		max_size_len = fmax(size_len, max_size_len);

		file_list[file_itr++] = data_obj;
	}

	qsort(file_list, (size_t)file_itr, sizeof(struct file_data*), cmp);

	for (size_t i = 0; i < (size_t)file_itr; i++) {
		print_info(file_list[i], max_link_len, max_user_name_len,
				max_group_name_len, max_size_len);
	}
}

//int main(int argc, char* argv[]) {
int main(void){
	show(".", 1);

	return 1;
}

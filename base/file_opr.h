#ifndef _FILE_OPR_H_
#define _FILE_OPR_H_

#include <sys/stat.h>
#include <sys/types.h>



namespace file 
{

enum open_mode
{
    RDONLY,
    RDWR
};

typedef int (*dir_traverse_func)(const char *file_path, struct stat *statbuf, void *arg);

typedef int (*for_each_dir_cb)(const char *name, struct stat *statbuf, void *arg);

int for_each_in_dir(const char *dir_path, for_each_dir_cb dir_cb, void *arg);

int is_file_exist(const char *file_path);

bool is_dir_exist(const char *dir_path);

int make_dir(const char *dir_path);

int create_file(const char *path);

int open(const char *path, open_mode mode);

ssize_t readn(int fd, char *buf, size_t count);

ssize_t writen(int fd, const char *buf, size_t count);


int close(int fd);

int rm(const char *path);

}


#endif

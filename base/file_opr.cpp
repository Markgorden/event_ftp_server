#include "base/file_opr.h"
#include "base/error.h"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


/*
int _dir_traverse(FILE *fp_read, FILE *fp_write, dir_traverse_func func, void *arg)
{
	char dir_path[512] = {0};
	int flushed = 0;
	int dir_quantity = 0;
	int ret;

retry:
	while(fgets(dir_path, sizeof(dir_path), fp_read) != NULL)
	{
		struct dirent *dirp;
		DIR  *dp;

		flushed = 0;

		dir_path[strlen(dir_path) - 1] = 0; //eliminate the '\n'
		if((dp = opendir(dir_path)) == NULL)
		{
			log_warning("cannot find dir %s", dir_path);
			return EOPEN;
		}

		while((dirp = readdir(dp)) != NULL)
		{
			const char *name = dirp->d_name;
			char path[512];
			struct stat statbuf;

			if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
			{
				continue;
			}

			snprintf(path, sizeof(path), "%s/%s", dir_path, name);

			if(lstat(path, &statbuf) < 0)
			{
				log_warning("lstat for %s error", path);
				closedir(dp);
				return ESTATFS;
			}

			if(S_ISDIR(statbuf.st_mode))
			{
				save_dir_path(path, fp_write);
				dir_quantity++;
				if(dir_quantity >= QUANTITY_PER_FLUSH)
				{
					dir_quantity = 0;
					fflush(fp_write);
				}
			}
			else
			{
				log_debug("find file(%s),handle it", path);
				if((ret = func(path, &statbuf, arg)) != 0)
				{
					log_warning("traverse cb error,0x%x", ret);
					return ret;
				}
			}
		}

		closedir(dp);	
	}

	if(flushed == 0)
	{
		fflush(fp_write);
		dir_quantity = 0;
		flushed = 1;
		goto retry;
	}

	DBG("exit");

	return RET_SUCCESS;
}


int dir_traverse(const char *dir_path, dir_traverse_func func, void *arg, bool handle_dir)
{
	FILE *fp_write, *fp_read;
	int ret;
	
	fp_write = fopen(DIR_TRAVERSE_TMP_FILE, "w+");
	fp_read  = fopen(DIR_TRAVERSE_TMP_FILE, "r");
	if(fp_write == NULL || fp_read == NULL)
	{
		return -1;
	}

	save_dir_path(dir_path, fp_write);
	fflush(fp_write);

	if((ret = _dir_traverse(fp_read, fp_write, func, arg)) != RET_SUCCESS)
	{
		log_warning("traverse error(0x%x)",ret);
		fclose(fp_read);
		fclose(fp_write);
		return ret;
	}

	fclose(fp_read);
	fclose(fp_write);
	return 0;
}	

*/

static char* last_char_is(const char *s, int c)
{
    if (s && *s) {
        size_t sz = strlen(s) - 1;
        s += sz;
        if ( (unsigned char)*s == c)
            return (char*)s;
    }
    return NULL;
}



static char* concat_path_file(const char *path, const char *filename)
{
    char *lc;
    char *p;

    if (!path)
        path = "";
    lc = last_char_is(path, '/');
    while (*filename == '/')
                filename++;
    p = (char *)malloc(strlen(path)+strlen(filename)+2);
    sprintf(p,"%s%s%s", path, (lc==NULL ? "/" : ""), filename);

    return p;
}



int file::for_each_in_dir(const char *dir_path, for_each_dir_cb dir_cb, void *arg)
{
	DIR *p_dir;
	struct dirent *entry;
	int ret = 0;

	if(dir_path == NULL || dir_cb == NULL)
	{
		return ERR_INVALID_PARAM;
	}

	p_dir = opendir(dir_path);
	if(p_dir == NULL)
	{
		return ERR_INVALID_PARAM;
	}

	while((entry = readdir(p_dir)) != NULL)
	{
		const char *name = entry->d_name;
	    char path[512];
		struct stat statbuf;

		snprintf(path, sizeof(path), "%s/%s", dir_path, name);

		if(lstat(path, &statbuf) < 0)
		{
			return ERR_FS_STAT;
		}
		
		ret = (*dir_cb)(name, &statbuf, arg);	
		if(ret < 0)
		{
			break;
		}
	}

	closedir(p_dir);
	return ret;
}


int file::is_file_exist(const char *file_path)
{
    struct stat file_stat;

    if(stat(file_path, &file_stat) < 0)
    {
        if(errno == ENOENT)
            return false;

        fprintf(stderr, "stat error"); 
        return false;
    }

    if(S_ISREG(file_stat.st_mode) || S_ISLNK(file_stat.st_mode))
        return true;

    return false;
}



bool file::is_dir_exist(const char *dir_path)
{
    struct stat file_stat;

    if(stat(dir_path, &file_stat) < 0)
    {
        if(errno == ENOENT)
            return false;

        fprintf(stderr, "stat error");
        return false;
    }

    if(S_ISDIR(file_stat.st_mode))
        return true;

    return false;
}


int file::make_dir(const char *dir_path)
{
	int ret = is_dir_exist(dir_path);
	
	if(ret > 0)
	{
		return 0;
	}
	else if(ret == 0)
	{
		return mkdir(dir_path, 0751);
	}
	else
	{
		return -1;	
	}
}


int file::create_file(const char *path)
{
    return creat(path, 0777);
}


int file::open(const char *path, open_mode mode)
{
    mode_t m;

    if(mode == RDONLY)
        m = O_RDONLY;
    else
        m = O_RDWR;
    
    return ::open(path, m);
}


ssize_t file::readn(int fd, char *buf, size_t count)
{
    ssize_t ret = -1;
    size_t read_bytes = 0;

    while((ret = read(fd, buf+read_bytes, count-read_bytes)) != 0)
    {
        if(ret < 0)
        {
            if(errno == EINTR)
                continue;
            else
                break;
        }

        read_bytes += ret;
    }

    if(ret == 0)
        return read_bytes;
    else if(ret < 0)
        return -1;

    return read_bytes;
}



ssize_t file::writen(int fd, const char *buf, size_t count)
{
	ssize_t ret = -1;
	ssize_t write_bytes = 0;

	if(fd < 0)
		return -1;
	
	while(write_bytes < count)
	{
		ret = write(fd, buf+write_bytes, count-write_bytes); 
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;
			return write_bytes; 	
		}

		write_bytes += ret;
	}

	return write_bytes;
}


int file::close(int fd)
{
    return ::close(fd);
}


int file::rm(const char *path)
{
    struct stat path_stat;

    if(stat(path,&path_stat) < 0)
    {
        return ERR_FS_STAT;
    }

    //directory
    if(S_ISDIR(path_stat.st_mode))
    {
        DIR *dp;
        struct dirent *d;
        //int status = 0;

        dp = opendir(path);
        if (dp == NULL)
        {
    	    return ERR_FS_OPEN;
        }

        while((d = readdir(dp)) != NULL)
        {
            char *new_path;

            if(strcmp(d->d_name, ".") == 0
              || strcmp(d->d_name, "..") == 0)
            {
                continue;
            }
 
            new_path = concat_path_file(path, d->d_name);
            if(rm(new_path) != SUCCESS)
            {
                free(new_path);
                closedir(dp);
                return ERR_FS_RM;
            }
            free(new_path);
        } 
        closedir(dp);

        if (rmdir(path) < 0)
        {
	        return ERR_FS_RM;
        }

        return SUCCESS; 
    }
    //regular file
    if (unlink(path) < 0) 
    {
        return ERR_FS_RM;
    }
 
    return SUCCESS;
}



#ifndef _ERROR_H
#define _ERROR_H

enum common_error_code
{
    SUCCESS = 0,
	ERR_NOMEM,
	ERR_INVALID_PARAM,
};

enum fs_error_code
{
	ERR_FS = 0x10,
	ERR_FS_OPEN,
	ERR_FS_READ,
	ERR_FS_WRITE,
	ERR_FS_STAT,
	ERR_FS_RM,
};



#endif

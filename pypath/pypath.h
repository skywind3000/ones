//=====================================================================
//
// pypath.h - 
//
// Created by skywind on 2026/03/06
// Last Modified: 2026/03/06 17:07:09
//
//=====================================================================
#ifndef _PY_PATH_H_
#define _PY_PATH_H_

#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

//---------------------------------------------------------------------
// Global Definition
//---------------------------------------------------------------------
#if (defined(__APPLE__) && defined(__MACH__)) || defined(__MACOS__)
#ifndef __unix
#define __unix 1
#endif
#endif

#if defined(__unix__) || defined(unix) || defined(__linux)
#ifndef __unix
#define __unix 1
#endif
#endif

#include <stdio.h>

#ifdef __unix
#include <unistd.h>
#define IPATHSEP '/'
#else
#include <direct.h>
#if defined(_WIN32)
#define IPATHSEP '\\'
#else
#define IPATHSEP '/'
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// File Stat
//---------------------------------------------------------------------
#define ISTAT_IFMT		0170000		// file type mask
#define ISTAT_IFIFO		0010000		// named pipe (fifo) 
#define ISTAT_IFCHR		0020000		// charactor special
#define ISTAT_IFDIR		0040000		// directory
#define ISTAT_IFBLK		0060000		// block special
#define ISTAT_IFREG		0100000		// regular
#define ISTAT_IFLNK		0120000		// symbolic link
#define ISTAT_IFSOCK	0140000		// socket
#define ISTAT_IFWHT		0160000		// whiteout
#define ISTAT_ISUID		0004000		// set user id on execution
#define ISTAT_ISGID		0002000		// set group id on execution
#define ISTAT_ISVXT		0001000		// swapped text even after use
#define ISTAT_IRWXU		0000700		// owner RWX mask
#define ISTAT_IRUSR		0000400		// owner read permission
#define ISTAT_IWUSR		0000200		// owner writer permission
#define ISTAT_IXUSR		0000100		// owner execution permission
#define ISTAT_IRWXG		0000070		// group RWX mask
#define ISTAT_IRGRP		0000040		// group read permission
#define ISTAT_IWGRP		0000020		// group write permission
#define ISTAT_IXGRP		0000010		// group execution permission
#define ISTAT_IRWXO		0000007		// other RWX mask
#define ISTAT_IROTH		0000004		// other read permission
#define ISTAT_IWOTH		0000002		// other writer permission
#define ISTAT_IXOTH		0000001		// other execution permission

#define ISTAT_ISFMT(m, t)	(((m) & ISTAT_IFMT) == (t))
#define ISTAT_ISDIR(m)		ISTAT_ISFMT(m, ISTAT_IFDIR)
#define ISTAT_ISCHR(m)		ISTAT_ISFMT(m, ISTAT_IFCHR)
#define ISTAT_ISBLK(m)		ISTAT_ISFMT(m, ISTAT_IFBLK)
#define ISTAT_ISREG(m)		ISTAT_ISFMT(m, ISTAT_IFREG)
#define ISTAT_ISFIFO(m)		ISTAT_ISFMT(m, ISTAT_IFIFO)
#define ISTAT_ISLNK(m)		ISTAT_ISFMT(m, ISTAT_IFLNK)
#define ISTAT_ISSOCK(m)		ISTAT_ISFMT(m, ISTAT_IFSOCK)
#define ISTAT_ISWHT(m)		ISTAT_ISFMT(m, ISTAT_IFWHT)

struct PYPATH_STAT
{
	uint32_t st_mode;
	uint64_t st_ino;
	uint64_t st_dev;
	uint32_t st_nlink;
	uint32_t st_uid;
	uint32_t st_gid;
	uint64_t st_size;
	uint64_t atime;
	uint64_t mtime;
	uint64_t ctime;
	uint32_t st_blocks;
	uint32_t st_blksize;
	uint32_t st_rdev;
	uint32_t st_flags;
};

typedef struct PYPATH_STAT pypath_stat_t;

#define PYPATH_MAXPATH		4096
#define PYPATH_MAXBUFF		((PYPATH_MAXPATH) + 8)


// returns 0 for success, -1 for error
int pypath_stat(const char *path, pypath_stat_t *ostat);

// wide-char: returns 0 for success, -1 for error
int pypath_wstat(const wchar_t *path, pypath_stat_t *ostat);

// returns 0 for success, -1 for error
int pypath_lstat(const char *path, pypath_stat_t *ostat);

// returns 0 for success, -1 for error
int pypath_fstat(int fd, pypath_stat_t *ostat);

// get current directory
char *pypath_getcwd(char *path, int size);

// wide-char: get current directory (wide char)
wchar_t *pypath_wgetcwd(wchar_t *path, int size);

// create directory
int pypath_mkdir(const char *path, int mode);

// wide-char: create directory (wide char)
int pypath_wmkdir(const wchar_t *path, int mode);

// change directory
int pypath_chdir(const char *path);

// wide-char: change directory (wide char)
int pypath_wchdir(const wchar_t *path);


#ifndef F_OK
#define F_OK		0
#endif

#ifndef X_OK
#define X_OK		1
#endif

#ifndef W_OK
#define W_OK		2
#endif

#ifndef R_OK
#define R_OK		4
#endif

// check access
int pypath_access(const char *path, int mode);

// wide-char: check access (wide char)
int pypath_waccess(const wchar_t *path, int mode);

// returns 1 for true 0 for false, -1 for not exist
int pypath_isdir(const char *path);

// wide-char: returns 1 for true 0 for false, -1 for not exist
int pypath_wisdir(const wchar_t *path);

// returns 1 for true 0 for false, -1 for not exist
int pypath_isfile(const char *path);

// wide-char: returns 1 for true 0 for false, -1 for not exist
int pypath_wisfile(const wchar_t *path);

// returns 1 for true 0 for false, -1 for not exist
int pypath_islink(const char *path);

// wide-char: returns 1 for true 0 for false, -1 for not exist
int pypath_wislink(const wchar_t *path);

// returns 1 for true 0 for false
int pypath_exists(const char *path);

// wide-char: returns 1 for true 0 for false
int pypath_wexists(const wchar_t *path);

// returns file size, -1 for error
int64_t pypath_getsize(const char *path);

// returns file size, -1 for error
int64_t pypath_wgetsize(const wchar_t *path);



//---------------------------------------------------------------------
// Python Path
//---------------------------------------------------------------------

// check absolute path, returns 1 for true 0 for false
int pypath_isabs(const char *path);

// wide-char: check absolute path, returns 1 for true 0 for false
int pypath_wisabs(const wchar_t *path);

// get absolute path
char *pypath_abspath(const char *srcpath, char *path, int maxsize);

// wide-char: get absolute path
wchar_t *pypath_wabspath(const wchar_t *srcpath, wchar_t *path, int maxsize);

// normalize: remove redundant "./", "../" and duplicate separators
char *pypath_normal(const char *srcpath, char *path, int maxsize);

// wide-char: normalize: remove redundant "./", "../" and duplicate separators
wchar_t *pypath_wnormal(const wchar_t *srcpath, wchar_t *path, int maxsize);

// concatenate two paths
char *pypath_join(const char *p1, const char *p2, char *path, int len);

// wide-char: concatenate two paths
wchar_t *pypath_wjoin(const wchar_t *p1, const wchar_t *p2, 
	wchar_t *path, int len);

// get directory name from path
char *pypath_dirname(const char *path, char *dir, int maxsize);

// wide-char: get directory name from path
wchar_t *pypath_wdirname(const wchar_t *path, wchar_t *dir, int maxsize);

// get file name from path
char *pypath_basename(const char *path, char *file, int maxsize);

// wide-char: get file name from path
wchar_t *pypath_wbasename(const wchar_t *path, wchar_t *file, int maxsize);

// get file extension from path
char *pypath_extname(const char *path, char *ext, int maxsize);

// wide-char: get file extension from path
wchar_t *pypath_wextname(const wchar_t *path, wchar_t *ext, int maxsize);

// path split: find the last "/" from right to left, split into two parts
int pypath_split(const char *path, char *p1, int l1, char *p2, int l2);

// wide-char: path split: find the last "/" from right to left, split into two parts
int pypath_wsplit(const wchar_t *path, wchar_t *p1, int l1, wchar_t *p2, int l2);

// extension split: split file name and extension
int pypath_splitext(const char *path, char *p1, int l1, char *p2, int l2);

// extension split: split file name and extension
int pypath_wsplitext(const wchar_t *path, wchar_t *p1, int l1, wchar_t *p2, int l2);

// path case normalize (to lower case on Windows)
char *pypath_normcase(const char *srcpath, char *path, int maxsize);

// wide-char: path case normalize (to lower case on Windows)
wchar_t *pypath_wnormcase(const wchar_t *srcpath, wchar_t *path, int maxsize);

// common path, aka. longest common prefix, from two paths
int pypath_common(const char *p1, const char *p2, char *path, int maxsize);

// wide-char: common path, aka. longest common prefix, from two paths
int pypath_wcommon(const wchar_t *p1, const wchar_t *p2, wchar_t *path, int maxsize);

// get relative path from start to srcpath
char *pypath_relpath(const char *srcpath, const char *start, char *path, int maxsize);

// wide-char: get relative path from start to srcpath
wchar_t *pypath_wrelpath(const wchar_t *srcpath, const wchar_t *start, wchar_t *path, int maxsize);



#ifdef __cplusplus
}
#endif


#endif


//=====================================================================
//
// pypath.c - 
//
// Created by skywind on 2026/03/06
// Last Modified: 2026/03/06 17:07:12
//
//=====================================================================
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "pypath.h"


//---------------------------------------------------------------------
// Global Definition
//---------------------------------------------------------------------
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/sysctl.h>
#endif

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#define ISYSNAME 'w'
#else
#ifndef __unix
#define __unix
#endif
#define ISYSNAME 'u'
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif


//---------------------------------------------------------------------
// Posix Stat
//---------------------------------------------------------------------
#ifdef __unix
typedef struct stat pypath_ostat_t;
#define pypath_stat_proc	stat
#define pypath_lstat_proc	lstat
#define pypath_fstat_proc	fstat
#else
typedef struct _stat pypath_ostat_t;
#define pypath_stat_proc	_stat
#define pypath_wstat_proc	_wstat
#define pypath_lstat_proc	_stat
#define pypath_fstat_proc	_fstat
#endif


#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	#if defined(_S_IFMT) && (!defined(S_IFMT))
		#define S_IFMT _S_IFMT
	#endif

	#if defined(_S_IFDIR) && (!defined(S_IFDIR))
		#define S_IFDIR _S_IFDIR
	#endif

	#if defined(_S_IFCHR) && (!defined(S_IFCHR))
		#define S_IFCHR _S_IFCHR
	#endif

	#if defined(_S_IFIFO) && (!defined(S_IFIFO))
		#define S_IFIFO _S_IFIFO
	#endif

	#if defined(_S_IFREG) && (!defined(S_IFREG))
		#define S_IFREG _S_IFREG
	#endif

	#if defined(_S_IREAD) && (!defined(S_IREAD))
		#define S_IREAD _S_IREAD
	#endif

	#if defined(_S_IWRITE) && (!defined(S_IWRITE))
		#define S_IWRITE _S_IWRITE
	#endif

	#if defined(_S_IEXEC) && (!defined(S_IEXEC))
		#define S_IEXEC _S_IEXEC
	#endif
#endif

#define IX_FMT(m, t)  (((m) & S_IFMT) == (t))


// convert stat structure
void pypath_stat_convert(pypath_stat_t *ostat, const pypath_ostat_t *x)
{
	memset(ostat, 0, sizeof(pypath_stat_t));
	ostat->st_mode = 0;

	#ifdef S_IFDIR
	if (IX_FMT(x->st_mode, S_IFDIR)) ostat->st_mode |= ISTAT_IFDIR;
	#endif
	#ifdef S_IFCHR
	if (IX_FMT(x->st_mode, S_IFCHR)) ostat->st_mode |= ISTAT_IFCHR;
	#endif
	#ifdef S_IFBLK
	if (IX_FMT(x->st_mode, S_IFBLK)) ostat->st_mode |= ISTAT_IFBLK;
	#endif
	#ifdef S_IFREG
	if (IX_FMT(x->st_mode, S_IFREG)) ostat->st_mode |= ISTAT_IFREG;
	#endif
	#ifdef S_IFIFO
	if (IX_FMT(x->st_mode, S_IFIFO)) ostat->st_mode |= ISTAT_IFIFO;
	#endif
	#ifdef S_IFLNK
	if (IX_FMT(x->st_mode, S_IFLNK)) ostat->st_mode |= ISTAT_IFLNK;
	#endif
	#ifdef S_IFSOCK
	if (IX_FMT(x->st_mode, S_IFSOCK)) ostat->st_mode |= ISTAT_IFSOCK;
	#endif
	#ifdef S_IFWHT
	if (IX_FMT(x->st_mode, S_IFWHT)) ostat->st_mode |= ISTAT_IFWHT;
	#endif

#ifdef S_IREAD
	if (x->st_mode & S_IREAD) ostat->st_mode |= ISTAT_IRUSR;
#endif

#ifdef S_IWRITE
	if (x->st_mode & S_IWRITE) ostat->st_mode |= ISTAT_IWUSR;
#endif

#ifdef S_IEXEC
	if (x->st_mode & S_IEXEC) ostat->st_mode |= ISTAT_IXUSR;
#endif

#ifdef S_IRUSR
	if (x->st_mode & S_IRUSR) ostat->st_mode |= ISTAT_IRUSR;
	if (x->st_mode & S_IWUSR) ostat->st_mode |= ISTAT_IWUSR;
	if (x->st_mode & S_IXUSR) ostat->st_mode |= ISTAT_IXUSR;
#endif

#ifdef S_IRGRP
	if (x->st_mode & S_IRGRP) ostat->st_mode |= ISTAT_IRGRP;
	if (x->st_mode & S_IWGRP) ostat->st_mode |= ISTAT_IWGRP;
	if (x->st_mode & S_IXGRP) ostat->st_mode |= ISTAT_IXGRP;
#endif

#ifdef S_IROTH
	if (x->st_mode & S_IROTH) ostat->st_mode |= ISTAT_IROTH;
	if (x->st_mode & S_IWOTH) ostat->st_mode |= ISTAT_IWOTH;
	if (x->st_mode & S_IXOTH) ostat->st_mode |= ISTAT_IXOTH;
#endif
	
	ostat->st_size = (uint64_t)x->st_size;

	ostat->atime = (uint64_t)x->st_atime;
	ostat->mtime = (uint64_t)x->st_mtime;
	ostat->ctime = (uint64_t)x->st_ctime;

	ostat->st_ino = (uint64_t)x->st_ino;
	ostat->st_dev = (uint64_t)x->st_dev;
	ostat->st_nlink = (uint32_t)x->st_nlink;
	ostat->st_uid = (uint32_t)x->st_uid;
	ostat->st_gid = (uint32_t)x->st_gid;
	ostat->st_rdev = (uint32_t)x->st_rdev;

#ifdef __unix
//	#define IHAVE_STAT_ST_BLKSIZE
//	#define IHAVE_STAT_ST_BLOCKS
//	#define IHAVE_STAT_ST_FLAGS
#endif

#if defined(__unix)
	#ifdef IHAVE_STAT_ST_BLOCKS
	ostat->st_blocks = (uint32_t)x->st_blocks;
	#endif
	#ifdef IHAVE_STAT_ST_BLKSIZE
	ostat->st_blksize = (uint32_t)x->st_blksize;
	#endif
	#if !defined(__CYGWIN__) && defined(IHAVE_STAT_ST_FLAGS)
	ostat->st_flags = (uint32_t)x->st_flags;
	#endif
#endif
}

// returns 0 for success, -1 for error
int pypath_stat_imp(const char *path, pypath_stat_t *ostat)
{
	pypath_ostat_t xstat;
	int retval;
	retval = pypath_stat_proc(path, &xstat);
	if (retval != 0) return -1;
	pypath_stat_convert(ostat, &xstat);
	return 0;
}

// returns 0 for success, -1 for error
int pypath_wstat_imp(const wchar_t *path, pypath_stat_t *ostat)
{
#ifdef _WIN32
	pypath_ostat_t xstat;
	int retval;
	retval = pypath_wstat_proc(path, &xstat);
	if (retval != 0) return -1;
	pypath_stat_convert(ostat, &xstat);
	return 0;
#else
	int retval;
	char *buf = malloc(PYPATH_MAXPATH * 4 + 8);
	if (buf == NULL) return -1;
	wcstombs(buf, path, PYPATH_MAXPATH * 4 + 4);
	retval = pypath_stat_imp(buf, ostat);
	free(buf);
	return retval;
#endif
}

// returns 0 for success, -1 for error
int pypath_lstat_imp(const char *path, pypath_stat_t *ostat)
{
	pypath_ostat_t xstat;
	int retval;
	retval = pypath_lstat_proc(path, &xstat);
	if (retval != 0) return -1;
	pypath_stat_convert(ostat, &xstat);
	return 0;
}

// returns 0 for success, -1 for error
int pypath_fstat(int fd, pypath_stat_t *ostat)
{
	pypath_ostat_t xstat;
	int retval;
	retval = pypath_fstat_proc(fd, &xstat);
	if (retval != 0) return -1;
	pypath_stat_convert(ostat, &xstat);
	return 0;
}

// normalize stat path
static void pypath_os_stat(const char *src, char *dst)
{
	int size = (int)strlen(src);
	if (size > PYPATH_MAXPATH) size = PYPATH_MAXPATH;
	memcpy(dst, src, size + 1);
	if (size > 1) {
		int trim = 1;
		if (size == 3) {
			if (isalpha((int)dst[0]) && dst[1] == ':' && 
				(dst[2] == '/' || dst[2] == '\\')) trim = 0;
		}
		if (size == 1) {
			if (dst[0] == '/' || dst[0] == '\\') trim = 0;
		}
		if (trim) {
			if (dst[size - 1] == '/' || dst[size - 1] == '\\') {
				dst[size - 1] = 0;
				size--;
			}
		}
	}
}

// wide char version
static void pypath_os_wstat(const wchar_t *src, wchar_t *dst)
{
	int size = (int)wcslen(src);
	if (size > PYPATH_MAXPATH) size = PYPATH_MAXPATH;
	memcpy(dst, src, (size + 1) * sizeof(wchar_t));
	if (size > 1) {
		int trim = 1;
		if (size == 3) {
			if (isalpha((int)dst[0]) && dst[1] == L':' && 
				(dst[2] == L'/' || dst[2] == L'\\')) trim = 0;
		}
		if (size == 1) {
			if (dst[0] == L'/' || dst[0] == L'\\') trim = 0;
		}
		if (trim) {
			if (dst[size - 1] == L'/' || dst[size - 1] == L'\\') {
				dst[size - 1] = 0;
				size--;
			}
		}
	}
}


// returns 0 for success, -1 for error
int pypath_stat(const char *path, pypath_stat_t *ostat)
{
	char buf[PYPATH_MAXBUFF];
	pypath_os_stat(path, buf);
	return pypath_stat_imp(buf, ostat);
}

// wide-char: returns 0 for success, -1 for error
int pypath_wstat(const wchar_t *path, pypath_stat_t *ostat)
{
	wchar_t buf[PYPATH_MAXBUFF];
	pypath_os_wstat(path, buf);
	return pypath_wstat_imp(buf, ostat);
}

// returns 0 for success, -1 for error
int pypath_lstat(const char *path, pypath_stat_t *ostat)
{
	char buf[PYPATH_MAXBUFF];
	pypath_os_stat(path, buf);
	return pypath_lstat_imp(buf, ostat);
}

// get current directory
char *pypath_getcwd(char *path, int size)
{
#ifdef _WIN32
	return _getcwd(path, size);
#else
	return getcwd(path, size);
#endif
}

// wide-char: get current directory (wide char)
wchar_t *pypath_wgetcwd(wchar_t *path, int size)
{
#ifdef _WIN32
	return _wgetcwd(path, size);
#else
	char buf[PYPATH_MAXPATH + 8];
	char *ret = getcwd(buf, PYPATH_MAXPATH);
	if (ret == NULL) return NULL;
	mbstowcs(path, buf, size);
	return path;
#endif
}

// create directory
int pypath_mkdir(const char *path, int mode)
{
#ifdef _WIN32
	return _mkdir(path);
#else
	if (mode < 0) mode = 0755;
	return mkdir(path, mode);
#endif
}

// wide-char: create directory (wide char)
int pypath_wmkdir(const wchar_t *path, int mode)
{
#ifdef _WIN32
	return _wmkdir(path);
#else
	int hr;
	char *buf = (char*)malloc(PYPATH_MAXBUFF);
	if (buf == NULL) return -1;
	wcstombs(buf, path, PYPATH_MAXPATH);
	if (mode < 0) mode = 0755;
	hr = mkdir(buf, mode);
	free(buf);
	return hr;
#endif
}

// change directory
int pypath_chdir(const char *path)
{
#if defined(_WIN32) && (!defined(__BORLANDC__))
	return _chdir(path);
#else
	return chdir(path);
#endif
}

// wide-char: change directory (wide char)
int pypath_wchdir(const wchar_t *path)
{
#ifdef _WIN32
	return _wchdir(path);
#else
	char buf[PYPATH_MAXPATH + 8];
	wcstombs(buf, path, PYPATH_MAXPATH);
	return chdir(buf);
#endif
}

// check access
int pypath_access(const char *path, int mode)
{
#ifdef _WIN32
	return _access(path, mode);
#else
	return access(path, mode);
#endif
}

// wide-char: check access (wide char)
int pypath_waccess(const wchar_t *path, int mode)
{
#ifdef _WIN32
	return _waccess(path, mode);
#else
	char buf[PYPATH_MAXPATH + 8];
	wcstombs(buf, path, PYPATH_MAXPATH);
	return access(buf, mode);
#endif
}

// returns 1 for true 0 for false, -1 for not exist
int pypath_isdir(const char *path)
{
	pypath_stat_t s;
	int hr = pypath_stat(path, &s);
	if (hr != 0) return -1;
	return (ISTAT_ISDIR(s.st_mode))? 1 : 0;
}

// wide-char: returns 1 for true 0 for false, -1 for not exist
int pypath_wisdir(const wchar_t *path)
{
	pypath_stat_t s;
	int hr = pypath_wstat(path, &s);
	if (hr != 0) return -1;
	return (ISTAT_ISDIR(s.st_mode))? 1 : 0;
}

// returns 1 for true 0 for false, -1 for not exist
int pypath_isfile(const char *path)
{
	pypath_stat_t s;
	int hr = pypath_stat(path, &s);
	if (hr != 0) return -1;
	return (ISTAT_ISDIR(s.st_mode))? 0 : 1;
}

// wide-char: returns 1 for true 0 for false, -1 for not exist
int pypath_wisfile(const wchar_t *path)
{
	pypath_stat_t s;
	int hr = pypath_wstat(path, &s);
	if (hr != 0) return -1;
	return (ISTAT_ISDIR(s.st_mode))? 0 : 1;
}

// returns 1 for true 0 for false, -1 for not exist
int pypath_islink(const char *path)
{
	pypath_stat_t s;
	int hr = pypath_stat(path, &s);
	if (hr != 0) return -1;
	return (ISTAT_ISLNK(s.st_mode))? 1 : 0;
}

// wide-char: returns 1 for true 0 for false, -1 for not exist
int pypath_wislink(const wchar_t *path)
{
	pypath_stat_t s;
	int hr = pypath_wstat(path, &s);
	if (hr != 0) return -1;
	return (ISTAT_ISLNK(s.st_mode))? 1 : 0;
}

// returns 1 for true 0 for false
int pypath_exists(const char *path)
{
	pypath_stat_t s;
	int hr = pypath_stat(path, &s);
	if (hr != 0) return 0;
	return 1;
}

// wide-char: returns 1 for true 0 for false
int pypath_wexists(const wchar_t *path)
{
	pypath_stat_t s;
	int hr = pypath_wstat(path, &s);
	if (hr != 0) return 0;
	return 1;
}

// returns file size, -1 for error
int64_t pypath_getsize(const char *path)
{
	pypath_stat_t s;
	int hr = pypath_stat(path, &s);
	if (hr != 0) return -1;
	return (int64_t)s.st_size;
}

// returns file size, -1 for error
int64_t pypath_wgetsize(const wchar_t *path)
{
	pypath_stat_t s;
	int hr = pypath_wstat(path, &s);
	if (hr != 0) return -1;
	return (int64_t)s.st_size;
}



//---------------------------------------------------------------------
// Posix Path
//---------------------------------------------------------------------

// check if absolute path, returns 1 for true 0 for false
int pypath_isabs(const char *path)
{
	if (path == NULL) return 0;
	if (path[0] == '/') return 1;
	if (path[0] == 0) return 0;
#ifdef _WIN32
	if (path[0] == IPATHSEP) return 1;
	if (isalpha(path[0]) && path[1] == ':') {
		if (path[2] == '/' || path[2] == '\\') return 1;
	}
#endif
	return 0;
}

// wide-char: check absolute path, returns 1 for true 0 for false
int pypath_wisabs(const wchar_t *path)
{
	if (path == NULL) return 0;
	if (path[0] == L'/') return 1;
	if (path[0] == 0) return 0;
#ifdef _WIN32
	if (path[0] == IPATHSEP) return 1;
	if (isalpha((int)path[0]) && path[1] == L':') {
		if (path[2] == L'/' || path[2] == L'\\') return 1;
	}
#endif
	return 0;
}


//---------------------------------------------------------------------
// pypath_str_t - basic string definition
//---------------------------------------------------------------------
typedef struct {
	char *p;
	int l;
	int m;
}	pypath_str_t;


//---------------------------------------------------------------------
// pypath_str_t interface
//---------------------------------------------------------------------
#define _istrlen(s) ((s)->l)
#define pypath_str_charh(s, i) (((i) >= 0)? ((s)->p)[i] : ((s)->p)[(s)->l + (i)])

static char *pypath_str_init(pypath_str_t *s, char *p, int max)
{
	assert((max > 0) && p && s);
	s->p = p;
	s->l = 0;
	s->m = max;
	return p;
}

char *pypath_str_set(pypath_str_t *s, const char *p, int max)
{
	assert((max > 0) && p && s);
	s->p = (char*)p;
	s->l = (int)strlen(p);
	s->m = max;
	return (char*)p;
}

static char *pypath_str_cat(pypath_str_t *s, const char *p) 
{
	char *p1;

	assert(s && p);
	for (p1 = (char*)p; p1[0]; p1++, s->l++) {
		if (s->l >= s->m) break;
		s->p[s->l] = p1[0];
	}
	return s->p;
}

static char *pypath_str_copy(pypath_str_t *s, const char *p) 
{
	assert(s && p);
	s->l = 0;
	return pypath_str_cat(s, p);
}

static char *pypath_str_cats(pypath_str_t *s1, const pypath_str_t *s2) 
{
	int i;
	assert(s1 && s2);
	for (i = 0; i < s2->l; i++, s1->l++) {
		if (s1->l >= s1->m) break;
		s1->p[s1->l] = s2->p[i];
	}
	return s1->p;
}

static char *pypath_str_cstr(pypath_str_t *s) 
{
	assert(s);
	if (s->l >= s->m) s->l = s->m - 1;
	if (s->l < 0) s->l = 0;
	s->p[s->l] = 0;
	return s->p;
}

static char pypath_str_char(const pypath_str_t *s, int pos)
{
	if (pos >= 0) return (pos > s->l)? 0 : s->p[pos];
	return (pos < -(s->l))? 0 : s->p[s->l + pos];
}

static char pypath_str_charhop(pypath_str_t *s)
{
	char ch = pypath_str_char(s, -1);
	s->l--;
	if (s->l < 0) s->l = 0;
	return ch;
}

static char *pypath_str_chartok(pypath_str_t *s, const char *p)
{
	int i, k;

	assert(s && p);

	for (; _istrlen(s) > 0; ) {
		for (i = 0, k = 0; p[i] && k == 0; i++) {
			if (pypath_str_char(s, -1) == p[i]) k++;
		}
		if (k == 0) break;
		pypath_str_charhop(s);
	}
	for (; _istrlen(s) > 0; ) {
		for (i = 0, k = 0; p[i] && k == 0; i++) {
			if (pypath_str_char(s, -1) == p[i]) k++;
		}
		if (k) break;
		pypath_str_charhop(s);
	}

	return s->p;
}

static int pypath_str_charmp(pypath_str_t *s, const char *p)
{
	int i;
	for (i = 0; i < s->l && ((char*)p)[i]; i++)
		if (pypath_str_char(s, i) != ((char*)p)[i]) break;
	if (((char*)p)[i] == 0 && i == s->l) return 0;
	return 1;
}

static char *pypath_str_catc(pypath_str_t *s, char ch)
{
	char text[2] = " ";
	assert(s);
	text[0] = ch;
	return pypath_str_cat(s, text);
}

static int pypath_str_strtok(const char *p1, int *pos, const char *p2)
{
	int i, j, k, r;

	assert(p1 && pos && p2);

	for (i = *pos; p1[i]; i++) {
		for (j = 0, k = 0; p2[j] && k == 0; j++) {
			if (p1[i] == p2[j]) k++;
		}
		if (k == 0) break;
	}
	*pos = i;
	r = i;

	if (p1[i] == 0) return -1;
	for (; p1[i]; i++) {
		for (j = 0, k = 0; p2[j] && k == 0; j++) {
			if (p1[i] == p2[j]) k++;
		}
		if (k) break;
	}
	*pos = i;

	return r;
}


//---------------------------------------------------------------------
// normalize path
//---------------------------------------------------------------------
char *pypath_normal(const char *srcpath, char *path, int maxsize)
{
	int i, p, c, k, r, t = 0;
	pypath_str_t s1, s2;
	char *p1, *p2;
	char pp2[3];

	assert(srcpath && path && maxsize > 0);

	if (srcpath[0] == 0) {
		if (maxsize > 0) path[0] = 0;
		if (maxsize > 1) {
			path[0] = '.';
			path[1] = 0;
		}
		return path;
	}

	p1 = (char*)srcpath;

	path[0] = 0;
	pypath_str_init(&s1, path, maxsize);

	if (IPATHSEP == '\\') {
		pp2[0] = '/';
		pp2[1] = '\\';
		pp2[2] = 0;
	}	else {
		pp2[0] = '/';
		pp2[1] = 0;
	}

	p2 = pp2;

	if (p1[0] && p1[1] == ':' && (ISYSNAME == 'u' || ISYSNAME == 'w')) {
		pypath_str_catc(&s1, *p1++);
		pypath_str_catc(&s1, *p1++);
	}

	if (IPATHSEP == '/') {
		if (p1[0] == '/') pypath_str_catc(&s1, *p1++);
	}	
	else if (p1[0] == '/' || p1[0] == IPATHSEP) {
		pypath_str_catc(&s1, IPATHSEP);
		p1++;
	}

	r = (pypath_str_char(&s1, -1) == IPATHSEP)? 1 : 0;
	srcpath = (const char*)p1;	

	for (i = 0, c = 0, k = 0; (p = pypath_str_strtok(srcpath, &i, p2)) >= 0; k++) {
		s2.p = (char*)(srcpath + p);
		s2.l = s2.m = i - p;
		// _iputs(&s2); printf("*\n");
		if (pypath_str_charmp(&s2, ".") == 0) continue;
		if (pypath_str_charmp(&s2, "..") == 0) {
			if (c != 0) {
				pypath_str_chartok(&s1, (IPATHSEP == '\\')? "/\\:" : "/");
				c--;
				continue;
			}
			if (c == 0 && r) {
				continue;
			}
		}	else {
			c++;
		}
		t++;
		pypath_str_cats(&s1, &s2);
		pypath_str_catc(&s1, IPATHSEP);
	}
	if (_istrlen(&s1) == 0) {
		pypath_str_copy(&s1, ".");
	}	else {
		if (pypath_str_char(&s1, -1) == IPATHSEP && t > 0) 
			pypath_str_charhop(&s1);
	}
	return pypath_str_cstr(&s1);
}


//---------------------------------------------------------------------
// join path
//---------------------------------------------------------------------
char *pypath_join(const char *p1, const char *p2, char *path, int maxsize)
{
	pypath_str_t s;
	char cc;
	int postsep = 1;
	int len1;

	assert(p1 && p2 && maxsize > 0);

	pypath_str_init(&s, path, maxsize);

	if (p1 == NULL || p1[0] == 0) {
		if (p2 == NULL || p2[0] == 0) {
			if (maxsize > 0) path[0] = 0;
			return path;
		}
		else {
			pypath_str_cat(&s, p2);
			return pypath_str_cstr(&s);
		}
	}
	else if (p2 == NULL || p2[0] == 0) {
		len1 = (int)strlen(p1);
		cc = (len1 > 0)? p1[len1 - 1] : 0;
		if (cc == '/' || cc == '\\') {
			pypath_str_cat(&s, p1);
			return pypath_str_cstr(&s);
		} else {
			pypath_str_cat(&s, p1);
			pypath_str_catc(&s, IPATHSEP);
		}
		return pypath_str_cstr(&s);
	}
	else if (pypath_isabs(p2)) {
#ifdef _WIN32
		if (p2[0] == '\\' || p2[0] == '/') {
			if (p1[1] == ':') {
				pypath_str_catc(&s, p1[0]);
				pypath_str_catc(&s, ':');
				pypath_str_cat(&s, p2);
				return pypath_str_cstr(&s);
			}
		}
#endif
		pypath_str_cat(&s, p2);
		return pypath_str_cstr(&s);
	}
	else {
#ifdef _WIN32
		char d1 = (p1[1] == ':')? p1[0] : 0;
		char d2 = (p2[1] == ':')? p2[0] : 0;
		char m1 = (d1 >= 'A' && d1 <= 'Z')? (d1 - 'A' + 'a') : d1;
		char m2 = (d2 >= 'A' && d2 <= 'Z')? (d2 - 'A' + 'a') : d2;
		if (d1 != 0) {
			if (d2 != 0) {
				if (m1 == m2) {
					path[0] = d2;
					path[1] = ':';
					pypath_join(p1 + 2, p2 + 2, path + 2, maxsize - 2);
					return path;
				} else {
					pypath_str_cat(&s, p2);
					return pypath_str_cstr(&s);
				}
			}
		}
		else if (d2 != 0) {
			pypath_str_cat(&s, p2);
			return pypath_str_cstr(&s);
		}
#endif
	}

	postsep = 1;
	len1 = (int)strlen(p1);
	cc = (len1 > 0)? p1[len1 - 1] : 0;

	if (cc == '/') {
		postsep = 0;
	}   
	else {
#ifdef _WIN32
		if (cc == '\\') {
			postsep = 0;
		}
		else if (len1 == 2 && p1[1] == ':') {
			postsep = 0;
		}
#endif
	}

	pypath_str_cat(&s, p1);

	if (postsep) {
		pypath_str_catc(&s, IPATHSEP);
	}

	pypath_str_cat(&s, p2);

	return pypath_str_cstr(&s);
}


// absolute path for posix systems
char *pypath_abspath_unix(const char *srcpath, char *path, int maxsize)
{
	char buf[PYPATH_MAXBUFF * 2];
	char *base, *temp;
	base = buf;
	temp = base + PYPATH_MAXBUFF;
	if (base == NULL) return NULL;
	pypath_getcwd(base, PYPATH_MAXPATH);
	pypath_join(base, srcpath, temp, PYPATH_MAXPATH);
	pypath_normal(temp, path, maxsize);
	return path;
}

#ifdef _WIN32
char *pypath_abspath_win(const char *srcpath, char *path, int maxsize)
{
	char *fname;
	DWORD hr = GetFullPathNameA(srcpath, maxsize, path, &fname);
	if (hr == 0) return NULL;
	return path;
}

wchar_t *pypath_abspath_wwin(const wchar_t *srcpath, wchar_t *path, int maxsize)
{
	wchar_t *fname;
	DWORD hr = GetFullPathNameW(srcpath, (DWORD)maxsize, path, &fname);
	if (hr == 0) return NULL;
	return path;
}
#endif


// get absolute path
char *pypath_abspath(const char *srcpath, char *path, int maxsize)
{
#ifdef _WIN32
	char buf[PYPATH_MAXBUFF];
	if (pypath_abspath_win(srcpath, buf, PYPATH_MAXPATH) == NULL) {
		return NULL;
	}
	return pypath_normal(buf, path, maxsize);
#else
	return pypath_abspath_unix(srcpath, path, maxsize);
#endif
}

// wide-char: get absolute path
wchar_t *pypath_wabspath(const wchar_t *srcpath, wchar_t *path, int maxsize)
{
#ifdef _WIN32
	wchar_t buf[PYPATH_MAXBUFF];
	if (pypath_abspath_wwin(srcpath, buf, PYPATH_MAXPATH) == NULL) {
		return NULL;
	}
	return pypath_wnormal(buf, path, maxsize);
#else
	char temp[PYPATH_MAXBUFF];
	char *ret;
	wcstombs(temp, srcpath, PYPATH_MAXBUFF);
	ret = pypath_abspath_unix(temp, temp, PYPATH_MAXBUFF);
	if (ret == NULL) {
		if (maxsize > 0) path[0] = 0;
		return NULL;
	}
	mbstowcs(path, temp, maxsize);
	return path;
#endif
}


//---------------------------------------------------------------------
// UTF-8 to UTF-16
//---------------------------------------------------------------------
int pypath_utf8towc(wchar_t *dest, const char *src, int n) 
{
#ifdef _WIN32
	int required = 0, result = 0;
	if (src == NULL || (src && src[0] == 0)) {
		if (dest && n > 0) dest[0] = L'\0';
		return 0;
	}

	required = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);

	if (required == 0) return -1;
	if (dest == NULL) return required - 1;
	if (n == 0) return 0;

	result = MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, (int)n);
	if (result == 0) {
		if (n > 0) {
			dest[0] = L'\0';
		}
		return -1;
	}
	return result - 1;
#else
	int result = (int)mbstowcs(dest, src, n);
	return result;
#endif
}


//---------------------------------------------------------------------
// UTF-16 to UTF-8
//---------------------------------------------------------------------
int pypath_wcstoutf8(char *dest, const wchar_t *src, int n) {
#ifdef _WIN32
	int required, result = 0;

	if (src == NULL || (src && src[0] == 0)) {
		if (dest && n > 0) dest[0] = '\0';
		return 0;
	}

	required = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);

	if (required == 0) return -1;
	if (dest == NULL) return required - 1;
	if (n == 0) return 0;

	result = WideCharToMultiByte(CP_UTF8, 0, src, -1, dest, (int)n, NULL, NULL);
	if (result == 0) {
		if (n > 0) dest[0] = '\0';
		return -1;
	}

	return result - 1;
#else
	int result = (int)wcstombs(dest, src, n);
	return result;
#endif
}


//---------------------------------------------------------------------
// wide-char: normalize: remove redundant "./", "../" and duplicate separators
//---------------------------------------------------------------------
wchar_t *pypath_wnormal(const wchar_t *srcpath, wchar_t *path, int maxsize)
{
	char buf[PYPATH_MAXBUFF * 2 + 8];
	char *tmp = buf + PYPATH_MAXBUFF + 4;
	pypath_wcstoutf8(buf, srcpath, PYPATH_MAXBUFF);
	pypath_normal(buf, tmp, PYPATH_MAXBUFF);
	pypath_utf8towc(path, tmp, maxsize);
	return path;
}


//---------------------------------------------------------------------
// wide-char: concatenate two paths
//---------------------------------------------------------------------
wchar_t *pypath_wjoin(const wchar_t *p1, const wchar_t *p2, 
	wchar_t *path, int len)
{
	char buf1[PYPATH_MAXBUFF + 8];
	char buf2[PYPATH_MAXBUFF + 8];
	char tmp[PYPATH_MAXBUFF + 8];
	pypath_wcstoutf8(buf1, p1, PYPATH_MAXBUFF);
	pypath_wcstoutf8(buf2, p2, PYPATH_MAXBUFF);
	pypath_join(buf1, buf2, tmp, PYPATH_MAXBUFF);
	pypath_utf8towc(path, tmp, len);
	return path;
}

// get directory name from path
char *pypath_dirname(const char *path, char *dir, int maxsize)
{
	pypath_split(path, dir, maxsize, NULL, 0);
	return dir;
}

// wide-char: get directory name from path
wchar_t *pypath_wdirname(const wchar_t *path, wchar_t *dir, int maxsize)
{
	pypath_wsplit(path, dir, maxsize, NULL, 0);
	return dir;
}

// get file name from path
char *pypath_basename(const char *path, char *file, int maxsize)
{
	pypath_split(path, NULL, 0, file, maxsize);
	return file;
}

// wide-char: get file name from path
wchar_t *pypath_wbasename(const wchar_t *path, wchar_t *file, int maxsize)
{
	pypath_wsplit(path, NULL, 0, file, maxsize);
	return file;
}


//---------------------------------------------------------------------
// path split: find the last "/" from right to left, split into two parts
//---------------------------------------------------------------------
int pypath_split(const char *path, char *p1, int l1, char *p2, int l2)
{
	int length, i, k, root;
	length = (int)strlen(path);

	if (length == 0) {
		if (p1 && l1 > 0) p1[0] = 0;
		if (p2 && l2 > 0) p2[0] = 0;
		return 0;
	}

	for (i = length - 1; i >= 0; i--) {
		if (IPATHSEP == '/') {
			if (path[i] == '/') break;
		}	else {
			if (path[i] == '/' || path[i] == '\\') break;
			if (path[i] == ':') break;
		}
	}

	if (IPATHSEP == '/') {
		root = (i == 0) ? 1 : 0;
	}	else {
		if (i == 0) root = 1;
		else if (i == 2 && path[1] == ':') root = 1;
		else if (i == 1 && path[1] == ':') root = 1;
		else root = 0;
	}

	if (p1) {
		if (i < 0) {
			if (l1 > 0) p1[0] = 0;
		}	
		else if (root) {
			int size = (i + 1) < l1 ? (i + 1) : l1;
			memcpy(p1, path, size);
			if (size < l1) p1[size] = 0;
		}
		else {
			int size = i < l1 ? i : l1;
			memcpy(p1, path, size);
			if (size < l1) p1[size] = 0;
		}
	}

	k = length - i - 1;

	if (p2) {
		if (k <= 0) {
			if (l2 > 0) p2[0] = 0;
		}	else {
			int size = k < l2 ? k : l2;
			memcpy(p2, path + i + 1, size);
			if (size < l2) p2[size] = 0;
		}
	}

	return 0;
}


//---------------------------------------------------------------------
// wide-char: path split: find the last "/" from right to left, split into two parts
//---------------------------------------------------------------------
int pypath_wsplit(const wchar_t *path, wchar_t *p1, int l1, wchar_t *p2, int l2)
{
	int length, i, k, root;
	length = (int)wcslen(path);

	if (length == 0) {
		if (p1 && l1 > 0) p1[0] = 0;
		if (p2 && l2 > 0) p2[0] = 0;
		return 0;
	}

	for (i = length - 1; i >= 0; i--) {
		if (IPATHSEP == '/') {
			if (path[i] == L'/') break;
		}	else {
			if (path[i] == L'/' || path[i] == L'\\') break;
		}
	}

	if (IPATHSEP == '/') {
		root = (i == 0) ? 1 : 0;
	}	else {
		if (i == 0) root = 1;
		else if (i == 2 && path[1] == L':') root = 1;
		else if (i == 1 && path[1] == L':') root = 1;
		else root = 0;
	}

	if (p1) {
		if (i < 0) {
			if (l1 > 0) p1[0] = 0;
		}	
		else if (root) {
			int size = (i + 1) < l1 ? (i + 1) : l1;
			memcpy(p1, path, size * sizeof(wchar_t));
			if (size < l1) p1[size] = 0;
		}
		else {
			int size = i < l1 ? i : l1;
			memcpy(p1, path, size * sizeof(wchar_t));
			if (size < l1) p1[size] = 0;
		}
	}

	k = length - i - 1;

	if (p2) {
		if (k <= 0) {
			if (l2 > 0) p2[0] = 0;
		}	else {
			int size = k < l2 ? k : l2;
			memcpy(p2, path + i + 1, size * sizeof(wchar_t));
			if (size < l2) p2[size] = 0;
		}
	}

	return 0;
}


//---------------------------------------------------------------------
// extension split: split file name and extension
//---------------------------------------------------------------------
int pypath_splitext(const char *path, char *p1, int l1, char *p2, int l2)
{
	int length, i, k, size, sep;
	length = (int)strlen(path);

	if (length == 0) {
		if (p1 && l1 > 0) p1[0] = 0;
		if (p2 && l2 > 0) p2[0] = 0;
		return 0;
	}

	for (i = length - 1, k = length; i >= 0; i--) {
		if (IPATHSEP == '/') {
			if (path[i] == '/') break;
		}	else {
			if (path[i] == '/' || path[i] == '\\') break;
			if (path[i] == ':') break;
		}
		if (k == length && path[i] == '.') {
			k = i;
		}
	}

	sep = i;

	if (k <= sep) k = length;
	else {
		for (i = sep + 1; i < k; i++) {
			if (path[i] != '.') break;
		}
		if (i == k) {
			k = length;
		}
	}

	if (p1) {
		size = k < l1 ? k : l1;
		if (size > 0) memcpy(p1, path, size);
		if (size < l1) p1[size] = 0;
	}

	size = length - k;
	if (size < 0) size = 0;
	size = size < l2 ? size : l2;

	if (p2) {
		if (size > 0) memcpy(p2, path + k, size);
		if (size < l2) p2[size] = 0;
	}

	return 0;
}


//---------------------------------------------------------------------
// extension split: split file name and extension
//---------------------------------------------------------------------
int pypath_wsplitext(const wchar_t *path, wchar_t *p1, int l1, wchar_t *p2, int l2)
{
	int length, i, k, size, sep;
	length = (int)wcslen(path);

	if (length == 0) {
		if (p1 && l1 > 0) p1[0] = 0;
		if (p2 && l2 > 0) p2[0] = 0;
		return 0;
	}

	for (i = length - 1, k = length; i >= 0; i--) {
		if (IPATHSEP == L'/') {
			if (path[i] == L'/') break;
		}	else {
			if (path[i] == L'/' || path[i] == L'\\') break;
			if (path[i] == L':') break;
		}
		if (k == length && path[i] == L'.') {
			k = i;
		}
	}

	sep = i;

	if (k <= sep) k = length;
	else {
		for (i = sep + 1; i < k; i++) {
			if (path[i] != L'.') break;
		}
		if (i == k) {
			k = length;
		}
	}

	if (p1) {
		size = k < l1 ? k : l1;
		if (size > 0) memcpy(p1, path, size * sizeof(wchar_t));
		if (size < l1) p1[size] = 0;
	}

	size = length - k;
	if (size < 0) size = 0;
	size = size < l2 ? size : l2;

	if (p2) {
		if (size > 0) memcpy(p2, path + k, size * sizeof(wchar_t));
		if (size < l2) p2[size] = 0;
	}

	return 0;
}


//---------------------------------------------------------------------
// get file extension from path
//---------------------------------------------------------------------
char *pypath_extname(const char *path, char *ext, int maxsize)
{
	pypath_splitext(path, NULL, 0, ext, maxsize);
	return ext;
}


//---------------------------------------------------------------------
// wide-char: get file extension from path
//---------------------------------------------------------------------
wchar_t *pypath_wextname(const wchar_t *path, wchar_t *ext, int maxsize)
{
	pypath_wsplitext(path, NULL, 0, ext, maxsize);
	return ext;
}


// path case normalize (to lower case on Windows)
char *pypath_normcase(const char *srcpath, char *path, int maxsize)
{
	int size = (int)strlen(srcpath);
	int i;
	for (i = 0; i < size && i < maxsize - 1; i++) {
#ifdef _WIN32
		char ch = srcpath[i];
		if (ch >= 'A' && ch <= 'Z') 
			ch = (char)(ch - 'A' + 'a');
		path[i] = ch;
#else
		path[i] = srcpath[i];
#endif
	}
	if (i < maxsize) {
		path[i] = 0;
	}
	return path;
}


// wide-char: path case normalize (to lower case on Windows)
wchar_t *pypath_wnormcase(const wchar_t *srcpath, wchar_t *path, int maxsize)
{
	int size = (int)wcslen(srcpath);
	int i;
	for (i = 0; i < size && i < maxsize - 1; i++) {
#ifdef _WIN32
		wchar_t ch = srcpath[i];
		if (ch >= L'A' && ch <= L'Z')
			ch = (wchar_t)(ch - L'A' + L'a');
		path[i] = ch;
#else
		path[i] = srcpath[i];
#endif
	}
	if (i < maxsize) {
		path[i] = 0;
	}
	return path;
}


//---------------------------------------------------------------------
// common path, aka. longest common prefix, from two paths
//---------------------------------------------------------------------
int pypath_common(const char *p1, const char *p2, char *path, int maxsize)
{
	int size1, size2, length, i, k = 0;
	size1 = (int)strlen(p1);
	size2 = (int)strlen(p2);
	length = (size1 < size2) ? size1 : size2;

	for (i = 0; i < length; i++) {
		char ch1 = p1[i];
		char ch2 = p2[i];
#ifdef _WIN32
		if (ch1 >= 'A' && ch1 <= 'Z') ch1 = (char)(ch1 - 'A' + 'a');
		if (ch2 >= 'A' && ch2 <= 'Z') ch2 = (char)(ch2 - 'A' + 'a');
		if (ch1 == '\\') ch1 = '/';
		if (ch2 == '\\') ch2 = '/';
#endif
		if (ch1 == '/') {
			if (ch2 == '/') {
				k = i;
			}
			else {
				break;
			}
		}
#ifdef _WIN32
		else if (ch1 == ':') {
			if (ch2 == ':') {
				k = i + 1;
			}	else {
				break;
			}
		}
#endif
		else if (ch1 != ch2) {
			break;
		}
	}

	if (i == length) {
		if (size1 == size2) k = length;
		else if (size1 < size2) {
			if (p2[length] == '/' || p2[length] == '\\') {
				k = length;
			}
		}
		else if (size1 > size2) {
			if (p1[length] == '/' || p1[length] == '\\') {
				k = length;
			}
		}
	}

	if (length > 0) {
		if (k == 0) {
			if (p1[0] == '/' || p1[0] == '\\') {
				if (p2[0] == '/' || p2[0] == '\\') {
					k = 1;
				}
			}
		}
#ifdef _WIN32
		if (k == 2 && length >= 3) {
			if (p1[1] == ':' && p2[1] == ':') {
				if (p1[2] == '/' || p1[2] == '\\') {
					if (p2[2] == '/' || p2[2] == '\\') {
						k = 3;
					}
				}
			}
		}
#endif
	}

	if (path != NULL) {
		if (k > 0) {
			int n = k;
			if (n >= maxsize) n = maxsize - 1;
			memcpy(path, p1, n);
			path[n] = 0;
		}	else {
			if (maxsize > 0) path[0] = 0;
		}
	}

	return k;
}


//---------------------------------------------------------------------
// wide-char: common path, aka. longest common prefix, from two paths
//---------------------------------------------------------------------
int pypath_wcommon(const wchar_t *p1, const wchar_t *p2, wchar_t *path, int maxsize)
{
	char buf[PYPATH_MAXBUFF * 3 + 12];
	char *tmp = buf + PYPATH_MAXBUFF + 4;
	char *out = tmp + PYPATH_MAXBUFF + 4;
	pypath_wcstoutf8(buf, p1, PYPATH_MAXBUFF);
	pypath_wcstoutf8(tmp, p2, PYPATH_MAXBUFF);
	pypath_common(buf, tmp, out, PYPATH_MAXBUFF);
	pypath_utf8towc(path, out, maxsize);
	if (path == NULL) return 0;
	return (int)wcslen(path);
}


//---------------------------------------------------------------------
// get relative path from start to src, both must be absolute paths
//---------------------------------------------------------------------
int pypath_relative(const char *src, const char *start, char *path, int maxsize)
{
	int size1, size2, size3, length, i, k = 0;
	pypath_str_t output;
	if (!pypath_isabs(src)) return -1;
	if (!pypath_isabs(start)) return -2;
	size1 = (int)strlen(start);
	size2 = (int)strlen(src);
	length = (size1 < size2) ? size1 : size2;
	if (length >= 2) {
#ifdef _WIN32
		char d1 = (src[1] == ':') ? src[0] : 0;
		char d2 = (start[1] == ':') ? start[0] : 0;
		d1 = (d1 >= 'A' && d1 <= 'Z') ? (char)(d1 - 'A' + 'a') : d1;
		d2 = (d2 >= 'A' && d2 <= 'Z') ? (char)(d2 - 'A' + 'a') : d2;
		if (d1 != d2) {
			// different drive letters
			if (maxsize > 0) path[0] = 0;
			return -3;
		}
#endif
	}
	size3 = pypath_common(start, src, NULL, PYPATH_MAXPATH * 4);
	pypath_str_init(&output, path, maxsize);
	// count how many ".." needed
	k = 0;
	for (i = size3; i < size1; i++) {
		char ch = start[i];
		if (ch == '/' || ch == '\\') {
			k++;
		}
	}
	if (size1 > size3) {
		k++;
	}
	for (i = 0; i < k; i++) {
		if (output.l > 0) {
			pypath_str_catc(&output, IPATHSEP);
		}
		pypath_str_cat(&output, "..");
	}
	// append the rest part from src
	if (size2 > size3) {
		int padding = 0;
		if (output.l > 0) {
			pypath_str_catc(&output, IPATHSEP);
		}
		if (src[size3] == '/') {
			padding = 1;
		} else {
#ifdef _WIN32
			if (src[size3] == '\\') {
				padding = 1;
			}
			else if (src[size3] == ':') {
				padding = 1;
			}
#endif
		}
		pypath_str_cat(&output, src + size3 + padding);
	}
	pypath_str_cstr(&output);
	return 0;
}


//---------------------------------------------------------------------
// get relative path from start to srcpath
//---------------------------------------------------------------------
char *pypath_relpath(const char *srcpath, const char *start, char *path, int maxsize)
{
	int position;
	char buf1[PYPATH_MAXBUFF];
	char buf2[PYPATH_MAXBUFF];
	pypath_abspath(srcpath, buf1, PYPATH_MAXBUFF);
	srcpath = buf1;
	if (start == NULL || (start && start[0] == 0)) {
		pypath_getcwd(buf2, PYPATH_MAXPATH);
	} else {
		pypath_abspath(start, buf2, PYPATH_MAXPATH);
	}
	start = buf2;
	position = pypath_relative(srcpath, start, path, maxsize);
	if (position < 0) {
		if (maxsize > 0) path[0] = 0;
		return NULL;
	}
	return path;
}


//---------------------------------------------------------------------
// wide-char: get relative path from start to srcpath
//---------------------------------------------------------------------
wchar_t *pypath_wrelpath(const wchar_t *srcpath, const wchar_t *start, wchar_t *path, int maxsize)
{
	char *ret;
	char buf1[PYPATH_MAXBUFF];
	char buf2[PYPATH_MAXBUFF];
	char buf3[PYPATH_MAXBUFF];
	pypath_wcstoutf8(buf1, srcpath, PYPATH_MAXPATH);
	pypath_wcstoutf8(buf2, start, PYPATH_MAXPATH);
	ret = pypath_relpath(buf1, buf2, buf3, PYPATH_MAXPATH);
	if (ret == NULL) {
		if (maxsize > 0) path[0] = 0;
		return NULL;
	}
	pypath_utf8towc(path, buf3, maxsize);
	return path;
}



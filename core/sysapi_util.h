#ifndef __SYSAPI_UTIL_H__
#define __SYSAPI_UTIL_H__
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

#define SYSAPI_LEVEL_NORM 4
#define SYSAPI_LEVEL_INFO 3
#define SYSAPI_LEVEL_WARN 2
#define SYSAPI_LEVEL_ERR  1

void sysapi_printf(int level, char *msg, ...);

// log the informational message
// this will show up green on the console
#define sysapi_print_info(msg, ...) sysapi_printf(SYSAPI_LEVEL_INFO, msg " "__VA_ARGS__)

// log the warning message
// this will show up yellow on the console
#define sysapi_print_warn(msg, ...) sysapi_printf(SYSAPI_LEVEL_WARN, msg " "__VA_ARGS__)

// log the error message
// this will show up red on the console
#define sysapi_print_err(msg, ...) sysapi_printf(SYSAPI_LEVEL_ERR, msg " " __VA_ARGS__)

// log the normal informational message
// this will show up as a normal message on the console
#define sysapi_print_norm(msg, ...) sysapi_printf(SYSAPI_LEVEL_NORM, msg " " __VA_ARGS__)

#define sysapi_print_oom(msg) sysapi_printf(SYSAPI_LEVEL_ERR, msg "%s-> %s:%u  Out of memory\n", __FILE__, __func__, __LINE__)

#define sysapi_log_info sysapi_print_info

#define sysapi_log_warn sysapi_print_warn

#define sysapi_log_err sysapi_print_err

#define sysapi_log_norm sysapi_print_norm

#define SYSAPI_MIN_VAL(__a, __b) ((__a < __b) ? (__a): (__b))
#define SYSAPI_MAX_VAL(__a, __b) ((__a > __b) ? (__a): (__b))

// get a line from a file. The line will be copied to the buf.
// the last new line from the file will be eliminated from the buffer
int sysapi_get_line(char *buf, FILE *fp, int len);

// generate a random string of length len bytes
int sysapi_stringrand(char *elem, int len);

#define SYSAPI_ARCH_64_BIT 1
#define SYSAPI_ARCH_32_BIT 2

// get the current system architecture
int sysapi_get_arch(void);

// boolean API for the above
// find if the arch is of 64 bit
int sysapi_is_arch_64bit(void);

// find if the arch is of 32 bit
int sysapi_is_arch_32bit(void);

// daemonizing API
int sysapi_daemonize(char *wd, char *lfile);

// find if the file descriptor is a tty device
int sysapi_device_isatty(int fd);

// get the tty name of the file descriptor fd
int sysapi_get_ttyname(int fd, char *name, int len);

// generate a near pure random number from kernel device
int sysapi_get_rand(void);

// generate a random number with in the range 0 - range.
int sysapi_get_rand_range(int range);

int sysapi_create_lockfile(char *lfile);

int sapi_strvjoin(char *result,
		          int result_len,
				  char *delimiter, int n_strings, ...);

int sysapi_strempty(char *string);

char *sysapi_strrev(char *string, char *reverse, int rev_len);

int sysapi_find_files_with_ext(char *dir, char *ext,
                               void (*callback)(char *filename));

void sysapi_skip_line(FILE *fp);

int sysapi_getdelim(char *line, int size, char delim, FILE *fp);

sigset_t sysapi_init_siglock(int *signal_list, int signal_list_len);

int sysapi_signal_lock(sigset_t *set);

int sysapi_signal_unlock(sigset_t *set);

int sysapi_install_sighandler(int signal_no, void (*signal_callback)(int signal_no));

int sysapi_get_uint(char *data, int *err, unsigned long int *val);

void sysapi_stack_trace(void);

int sysapi_get_int(char *data, int *err, long int *val);

#endif


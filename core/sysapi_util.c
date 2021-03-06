#include <stdio.h>
#include <stdarg.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "sysapi_fs.h"
#include "sysapi_util.h"
#include "sysapi_libgen.h"

#define ANSI_COLOR_RED     "\x1B[31m"
#define ANSI_COLOR_GREEN   "\x1B[32m"
#define ANSI_COLOR_YELLOW  "\x1B[33m"
#define ANSI_COLOR_BLUE    "\x1B[34m"
#define ANSI_COLOR_MAGENTA "\x1B[35m"
#define ANSI_COLOR_CYAN    "\x1B[36m"
#define ANSI_COLOR_RESET   "\033[0m"

static void sysapi_print_in_red(char *msg, va_list ap)
{
    char str[1024];
    int len;

    len = snprintf(str, sizeof(str), "[ERR:   ] ");
    vsnprintf(str + len, sizeof(str) - len, msg, ap);
    fprintf(stderr, ANSI_COLOR_RED"%s"ANSI_COLOR_RESET, str);
}

static void sysapi_print_in_yellow(char *msg, va_list ap)
{
    char str[1024];
    int len;

    len = snprintf(str, sizeof(str), "[WARN:  ] ");
    vsnprintf(str + len, sizeof(str) - len, msg, ap);
    fprintf(stdout, ANSI_COLOR_YELLOW"%s"ANSI_COLOR_RESET, str);
}

static void sysapi_print_in_green(char *msg, va_list ap)
{
    char str[1024];
    int len;

    len = snprintf(str, sizeof(str), "[INFO:  ] ");
    vsnprintf(str + len, sizeof(str) - len, msg, ap);
    fprintf(stdout, ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET, str);
}

static void sysapi_print_in_norm(char *msg, va_list ap)
{
    char str[1024];
    int len;

    len = snprintf(str, sizeof(str), "[INFO:  ] ");
    vsnprintf(str + len, sizeof(str) - len, msg, ap);
    fprintf(stdout, "%s", str);
}

// do not use this API .. use sysapi_log_* macros instead for logging on to the console
void sysapi_printf(int level, char *msg, ...)
{
    va_list ap;
    struct level_printer {
        int level;
        void (*printer)(char *msg, va_list ap);
    } printers[] = {
        {SYSAPI_LEVEL_ERR,  sysapi_print_in_red},
        {SYSAPI_LEVEL_WARN, sysapi_print_in_yellow},
        {SYSAPI_LEVEL_INFO, sysapi_print_in_green},
        {SYSAPI_LEVEL_NORM, sysapi_print_in_norm},
    };
    int level_printer_size = sizeof(printers) / sizeof(printers[0]);

    va_start(ap, msg);

    if (level < 0 || level > level_printer_size)
        level = SYSAPI_LEVEL_NORM + 1;

    printers[level - 1].printer(msg, ap);

    va_end(ap);
}

int sysapi_get_arch(void)
{
    if ((size_t)-1 > 0xffffffffUL)
        return SYSAPI_ARCH_64_BIT;
    else
        return SYSAPI_ARCH_32_BIT;
}

int sysapi_is_arch_64bit(void)
{
    return (sysapi_get_arch() == SYSAPI_ARCH_64_BIT);
}

int sysapi_is_arch_32bit(void)
{
    return (sysapi_get_arch() == SYSAPI_ARCH_32_BIT);
}

int sysapi_stringrand(char *elem, int len)
{
    char alphabets[] = {
        'a', 'A',
        'b', 'B',
        'c', 'C',
        'd', 'D',
        'e', 'E',
        'f', 'F',
        'g', 'G',
        'h', 'H',
        'i', 'I',
        'j', 'J',
        'k', 'K',
        'l', 'L',
        'm', 'M',
        'n', 'N',
        'o', 'O',
        'p', 'P',
        'q', 'Q',
        'r', 'R',
        's', 'S',
        't', 'T',
        'u', 'U',
        'v', 'V',
        'w', 'W',
        'x', 'X',
        'y', 'Y',
        'z', 'Z',
    };
    int i;
    int seed;
    struct timeval tv;
    int size_alphabets = sizeof(alphabets) / sizeof(alphabets[0]);

    gettimeofday(&tv, 0);
    seed = tv.tv_usec;

    srand(seed);

    for (i = 0; i < len - 1; i++)
        elem[i] = alphabets[rand() % size_alphabets];

    elem[i] = '\0';

    return 0;
}

int sysapi_get_rand(void)
{
    int fd;
    int ret;
    int number = 0;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        sysapi_err(
                "%s-> %s:%d  cannot open /dev/urandom\n",
                __FILE__, __func__, __LINE__);
        return -1;
    }

    ret = read(fd, &number, sizeof(number));
    if (ret < 0) {
        sysapi_err(
                "%s-> %s:%d  cannot read from fd %d\n",
                __FILE__, __func__, __LINE__);
        close(fd);
        return -1;
    }

    close(fd);

    return number;
}

int sysapi_get_rand_range(int range)
{
    int ret;

    ret = sysapi_get_rand();
    if (ret != -1) {
        if (ret < 0)
            ret *= -1;
        return ret % range;
    }

    return ret;
}

int sapi_strvjoin(char *result, int result_len, char *delimiter, int n_strings, ...)
{
    va_list ap;
    int len = 0;
    int off = 0;
    int i;

    va_start(ap, n_strings);

    for (i = 0; i < n_strings; i++) {
        char *_item = va_arg(ap, char *);
        if (_item) {
            len = snprintf(result + off, result_len, "%s", _item);
            off += len;
            if (delimiter) {
                len = snprintf(result + off, result_len, "%s", delimiter);
                off += len;
            }
        }
    }

    va_end(ap);
    
    return off;
}

int sysapi_strempty(char *string)
{
    return string ? (!strcmp(string, "") ? 1: 0): 1;
}

char *sysapi_strrev(char *string, char *reverse, int rev_len)
{
    int len = strlen(string);
    int i, j;

    for (i = len, j = 0; i > 0 && j < rev_len; i--, j++)
        reverse[j] = string[i];

    if (j >= rev_len)
        j = rev_len -1;

    reverse[j] = '\0';
    return reverse;
}

sigset_t sysapi_init_siglock(int *signal_list, int signal_list_len)
{
    int i;
    sigset_t sigmask;
    
    sigemptyset(&sigmask);
    
    for (i = 0; i < signal_list_len; i++) {
        sigaddset(&sigmask, signal_list[i]);
    }
    return sigmask;
}

int sysapi_signal_lock(sigset_t *sigset)
{
    sigprocmask(SIG_BLOCK, sigset, NULL);
    return 0;
}

int sysapi_signal_unlock(sigset_t *sigset)
{
    sigprocmask(SIG_UNBLOCK, sigset, NULL);
    return 0;
}

int sysapi_get_int(char *data, int *err, long int *val)
{
    char *endptr;

    *val = strtol(data, &endptr, 10);
    if (endptr) {
        *err = -1;
        return -1;
    }

    *err = 0;
    return 0;
}

// atoi for strings return 0 but this atoi here is more
// perfect kind .. in the sense that it fails by showing
// that the return value is 0 instead of a -1 (just like atoi)

int sysapi_atoi(char *data)
{
    int val;
    int err;

    sysapi_get_int(data, &err, &val);
    if (err != -1)
        return val;
    return 0;
}

int sysapi_get_uint(char *data, int *err, unsigned long int *val)
{
    char *endptr;

    *val = strtoul(data, &endptr, 10);
    if (endptr) {
        *err = -1;
        return -1;
    }

    *err = 0;
    return 0;
}

int sysapi_get_double(char *data, double *val)
{
    char *endptr;

    *val = strtod(data, &endptr);
    if (endptr)
        return -1;

    return 0;
}

/// XXX: using of signal() API is outdated... should not be used
int sysapi_install_sighandler(int signal_no, void (*signal_callback)(int signal_no))
{
    signal(SIGINT, signal_callback);
    return 0;
}

void sysapi_stack_trace(void)
{
    int j, nptrs;
    void *buffer[200];
    char **strings;

    nptrs = backtrace(buffer, 200);

    strings = backtrace_symbols(buffer, nptrs);
    if (!strings)
        return;

    for (j = 0; j < nptrs; j++)
        fprintf(stderr, "%s\n", strings[j]);

    free(strings);
}


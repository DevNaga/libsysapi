#include "sysapi_fs.h"
#include "sysapi_fs_i.h"

int sysapi_dir_read(char *dirpath,
                    void (*callback)(char *filename, sysapi_file_type type, void *app_ctx),
                    void *app_ctx)
{
    DIR *dirp;
    struct dirent *entry;
    char path[SYSAPI_PATH_MAX];

    dirp = opendir(dirpath);
    if (!dirp)
        return -1;

    strncpy(path, dirpath, strlen(dirpath) + 1);
    while ((entry = readdir(dirp)) != NULL) {
        sysapi_file_type type;

        memset(path, 0, sizeof(path));
        strncat(path, entry->d_name, sizeof(path));

        struct stat sf;

        stat(path, &sf);

        if (S_ISREG(sf.st_mode))
            type = SYSAPI_FILE_TYPE_REGFILE;

        if (S_ISDIR(sf.st_mode))
            type = SYSAPI_FILE_TYPE_DIRECT;

        if (type == SYSAPI_FILE_TYPE_REGFILE ||
            type == SYSAPI_FILE_TYPE_DIRECT)
            callback(path, type, app_ctx);
    }

    closedir(dirp);
    return 0;
}

int sysapi_list_dir(char *dirpath,
                    void (*callback)(char *dirname, void *ctx),
                    void *ctx)
{
    DIR *dirp;
    struct dirent *entry;
    char path[SYSAPI_PATH_MAX];

    dirp = opendir(dirpath);
    if (!dirp)
        return -1;

    while ((entry = readdir(dirp)) != NULL) {
        memset(path, 0, sizeof(path));

        struct stat sf;

        stat(entry->d_name, &sf);

        if (S_ISDIR(sf.st_mode))
            callback(entry->d_name, ctx);
    }

    closedir(dirp);
    return 0;
}

int sysapi_create_symlink(char *original_file_path, char *symlink_path)
{
    return symlink(original_file_path, symlink_path);
}

int sysapi_describe_link(char *linkpath, char *actualname, int actual_len)
{
    ssize_t ret;

    ret = readlink(linkpath, actualname, actual_len);
    if (ret > 0)
        actualname[ret] = '\0';
    return ret;
}

int sysapi_create_pidfile(char *filename)
{
    pid_t pid;
    char buff[31];
    int fd;

    pid = getpid();

    snprintf(buff, sizeof(buff), "%d", pid);

    fd = open(filename, O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0)
        return -1;

    write(fd, buff, strlen(buff));
//    unlink(filename);
    close(fd);

    return 0;
}

int sysapi_unlink_file(char *filename)
{
    return unlink(filename);
}

int sysapi_touch(char *filename)
{
    int fd;

    fd = open(filename, O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0)
        return -1;

    close(fd);
    return 0;
}

int sysapi_truncate_file(char *filename, int filesize)
{
    return truncate(filename, filesize);
}

int sysapi_new_truncate_file(char *filename, int filesize)
{
    if (sysapi_touch(filename) == 0)
        return sysapi_truncate_file(filename, filesize);
    return -1;
}

int sysapi_untouch(char *filename)
{
    return sysapi_unlink_file(filename);
}

int sysapi_makedir(char *dirname)
{
    int ret;

    ret = mkdir(dirname, S_IRWXU);
    if (ret < 0)
        return -1;

    return 0;
}

int sysapi_dir_walk(char *dirpath,
                    void (*callback)(char *parent, sysapi_file_type file_type, char *filename, void *app_ctx),
                    void *app_ctx)
{
    DIR *dirp;
    struct dirent *entry;
    char path[SYSAPI_PATH_MAX];

    dirp = opendir(dirpath);
    if (!dirp)
        return -1;

    strncpy(path, dirpath, strlen(dirpath) + 1);
    while ((entry = readdir(dirp)) != NULL) {
        memset(path, 0, sizeof(path));
        strncpy(path, dirpath, sizeof(path));

        if (path[strlen(path) - 1] != '/')
            strncat(path, "/", sizeof(path));

        strncat(path, entry->d_name, sizeof(path));

        struct stat sf;

        stat(path, &sf);

        if (S_ISREG(sf.st_mode)) {
            callback(dirpath, SYSAPI_FILE_TYPE_REGFILE, path, app_ctx);
        } else if (S_ISDIR(sf.st_mode)) {
            if (path[strlen(path) - 1] != '/')
                strcat(path, "/");
            if (!strcmp(entry->d_name, ".") ||
                !strcmp(entry->d_name, "..")) {
            } else {
                callback(dirpath, SYSAPI_FILE_TYPE_DIRECT, path, app_ctx);
                sysapi_dir_walk(path, callback, app_ctx);
            }
        }
    }

    closedir(dirp);
    return 0;
}

int sysapi_read_binfile(char *filename,
                     void (*callback)(char *data, int len, void *app_ctx),
                     void *app_ctx)
{
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
        return -1;

    while (1) {
        char filedata[SYSAPI_FILE_MAX];

        memset(filedata, 0, sizeof(filedata));
        int ret = read(fd, filedata, sizeof(filedata));
        if (ret > 0)
            callback(filedata, ret, app_ctx);
        else
            break;
    }

    close(fd);
    return 0;
}

int sysapi_file_exist(char *filename)
{
    return access(filename, F_OK);
}

int sysapi_get_filesize(char *filename)
{
    struct stat st;
    int ret;

    ret = stat(filename, &st);
    if (ret < 0)
        return ret;

    return st.st_size;
}

int sysapi_get_symlink_count(char *filename)
{
    struct stat st;
    int ret;

    ret = stat(filename, &st);
    if (ret < 0)
        return ret;

    return st.st_nlink;
}

struct _sysapi_fmap {
    void *maped_mem;
    int f_size;
    int f_off;
    int fd;
};

static void *_sysapi_map_file(char *filename, int mode)
{
    struct _sysapi_fmap *_sfmap;
    struct stat _sfs;
    int ret;
    int file_mode = PROT_READ;

    _sfmap = calloc(1, sizeof(struct _sysapi_fmap));
    if (!_sfmap)
        return NULL;

    _sfmap->fd = open(filename, mode);
    if (_sfmap->fd < 0)
        goto err;

    ret = stat(filename, &_sfs);
    if (ret == -1)
        goto err;

    _sfmap->f_size = _sfs.st_size;

    if (mode == O_RDWR) {
        file_mode |= PROT_WRITE;
    }

    _sfmap->maped_mem = mmap(0, _sfmap->f_size, mode, MAP_SHARED, _sfmap->fd, 0);
    if (_sfmap->maped_mem == MAP_FAILED)
        goto err;

    return _sfmap;
err:
    if (_sfmap) {
        if (_sfmap->fd > 0)
            close(_sfmap->fd);
        free(_sfmap);
    }
    return NULL;
}

void *sysapi_map_file_rdwr(char *filename)
{
    return _sysapi_map_file(filename, O_RDWR);
}

void *sysapi_map_file_rd(char *filename)
{
    return _sysapi_map_file(filename, O_RDONLY);
}

void sysapi_unmap_file(void *sfmap)
{
    struct _sysapi_fmap *_sfmap = sfmap;

    munmap(_sfmap->maped_mem, _sfmap->f_size);
    close(_sfmap->fd);
    free(_sfmap);
}

static void _sysapi_unmap_file(void *sfmap, int sync_mode)
{
    struct _sysapi_fmap *_sfmap = sfmap;

    msync(_sfmap->maped_mem, _sfmap->f_size, sync_mode);
    munmap(_sfmap->maped_mem, _sfmap->f_size);
    close(_sfmap->fd);
    free(_sfmap);
}

static void _sysapi_sync_file(void *sfmap, int fsize, int sync_mode)
{
    struct _sysapi_fmap *_sfmap = sfmap;

    msync(_sfmap->maped_mem, fsize, sync_mode);
}

void sysapi_async_file(void *sfmap, int fsize)
{
    _sysapi_sync_file(sfmap, fsize, MS_ASYNC);
}

void sysapi_sync_file(void *sfmap, int fsize)
{
    _sysapi_sync_file(sfmap, fsize, MS_SYNC);
}

void sysapi_sync_unmap_file(void *sfmap)
{
    _sysapi_unmap_file(sfmap, MS_SYNC);
}

void sysapi_async_unmap_file(void *sfmap)
{
    _sysapi_unmap_file(sfmap, MS_ASYNC);
}

void *sysapi_get_maped_fdata_ptr(void *sfmap)
{
    struct _sysapi_fmap *_sfmap = sfmap;

    return _sfmap->maped_mem + _sfmap->f_off;
}

void *sysapi_create_maped_file(char *filename, int size)
{
    int ret;

    ret = sysapi_new_truncate_file(filename, size);
    if (ret < 0)
        return NULL;

    return _sysapi_map_file(filename, O_RDWR);
}

void sysapi_close_maped_file(void *__sfmap)
{
    sysapi_sync_unmap_file(__sfmap);
}

int sysapi_update_writen_bytes(int len, void *__sfmap)
{
    struct _sysapi_fmap *_sfmap = __sfmap;

    if ((_sfmap->f_off + len) > _sfmap->f_size)
        return -1;

    _sfmap->f_off += len;

    return 0;
}

int sysapi_chroot_dir(char *directory)
{
    int ret;

    ret = chdir(directory);
    if (ret == 0)
        return chroot(directory);

    return -1;
}

void sysapi_skip_line(FILE *fp)
{
    while (fgetc(fp) != '\n');
}

int sysapi_getdelim(char *line, int size, char delim, FILE *fp)
{
    int c;
    int char_count = 0;

    while (1) {
        c = fgetc(fp);
        if ((c != EOF) && (c != delim)) {
            if (char_count < size)
                line[char_count] = c;
            else
                line[char_count] = '\0';
            char_count++;
        } else {
            line[char_count] = '\0';
            break;
        }
    }

    return char_count;
}

int sysapi_get_line(char *buf, FILE *fp, int len)
{
    int i = 0;
    int a;
    int length = 0;

    while (i < len - 1) {
        a = fgetc(fp);
        if (a == '\n') {
            length = i;
            break;
        }

        if (a == EOF) {
            length = -1;
            break;
        }

        buf[i] = a;
        i++;
    }

    buf[i] = '\0';

    return length;
}

int sysapi_create_lockfile(char *lfile)
{
    int fd;

    fd = open(lfile, O_RDWR | O_CREAT);
    if (lockf(fd, F_TLOCK, 0) < 0)
        return -1;
    return 0;
}

int sysapi_device_isatty(int fd)
{
    return isatty(fd);
}

int sysapi_get_ttyname(int fd, char *name, int len)
{
    return ttyname_r(fd, name, len);
}

struct _sysapi_internal_file_ext {
    char *filext;
    int found;
    void (*callback)(char *filename);
};

static void _sysapi_file_checker(char *filename,
                          sysapi_file_type ftype,
                          void *ctx)
{
    struct _sysapi_internal_file_ext *__fext = ctx;
    char *end;
    char ext[30] = {};
    char ext_new[30] = {};
    int i = 0;

    if (ftype == SYSAPI_FILE_TYPE_REGFILE) {
        end = filename + strlen(filename);
        while ((*end != '.') && (i < sizeof(ext))) {
            ext[i] = *end;
            end++;
            i++;
        }

        ext[i] = '\0';
        sysapi_strrev(ext, ext_new, sizeof(ext_new));

        if (strcmp(ext_new, __fext->filext) == 0) {
            __fext->found = 1;
            __fext->callback(filename);
        }
    }
}

int sysapi_find_files_with_ext(char *dir, char *ext,
                               void (*callback)(char *filename))
{
    struct _sysapi_internal_file_ext *_fext;
    int ret;

    _fext = calloc(1, sizeof(struct _sysapi_internal_file_ext));
    if (!_fext)
        return -1;

    _fext->found = 0;
    _fext->filext = ext;
    _fext->callback = callback;

    ret = sysapi_dir_read(dir, _sysapi_file_checker, _fext);
    if (!ret) {
        if (_fext->found)
            ret = 0;
        else
            ret = -1;
    }

    free(_fext);

    return ret;
}

#ifdef CONFIG_ADVANCED
// mini lsof command implementer API..
int sysapi_get_files_inuse(char *progname,
                           void (*callback)(char *filename, void *app_ctx),
                           void *app_ctx)
{
    DIR *dirp;
    struct dirent *entry;
    char path[SYSAPI_PATH_MAX];
}


struct sysapi_shmsys {
#define SAPI_SHM_SIZE 1024 * 10
    int size;
    void *start;
    void *shmaddr;
    int shmid;
    key_t key;
    int off;
    int read_off;
};

void* sapi_ramfs_create(int size)
{
    struct sysapi_shmsys *sys;

    sys = calloc(1, sizeof(*sys));
    if (!sys)
        return NULL;

    sys->size = SAPI_SHM_SIZE;

    sys->key = ftok("/tmp/", 'b');
    if (sys->key < 0)
        goto err_keygen;

    sys->shmid = shmget(sys->key, sys->size, 0666);
    if (sys->shmid < 0)
        goto err_shmget;

    sys->shmaddr = shmat(sys->shmid, NULL, 0);
    if (sys->shmaddr == (char *) -1)
        goto err_shmat;

    sys->start = sys->shmaddr;

    return sys;

err_shmat:
err_shmget:
err_keygen:
    free(sys);

    return NULL;
}

int sapi_ramfs_write(void *fs_data, void *data, int len)
{
    struct sysapi_shmsys *sys = fs_data;

    if (sys->off + len >= sys->size)
        return -1;

    memcpy(sys->shmaddr + sys->off, data, len);

    sys->off += len;

    return len;
}

int sapi_ramfs_read(void *fs_data, void *data, int len)
{
    struct sysapi_shmsys *sys = fs_data;

    if (len > (sys->size - sys->read_off))
        len = sys->size - sys->read_off;

    memcpy(data, sys->start + sys->read_off, len);

    return len;
}

#endif

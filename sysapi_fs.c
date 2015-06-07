#include "sysapi_fs.h"

int sysapi_dir_read(char *dirpath,
                    void (*callback)(char *filename, sysapi_file_type type, void *app_ctx),
                    void *app_ctx)
{
    DIR *dirp;
    struct dirent *entry;
    char path[300];

    dirp = opendir(dirpath);
    if (!dirp)
        return -1;

    strncpy(path, dirpath, strlen(dirpath) + 1);
    while ((entry = readdir(dirp)) != NULL) {
        sysapi_file_type type;

        memset(path, 0, sizeof(path));
        strncat(path, entry->d_name, strlen(entry->d_name));

        struct stat sf;

        stat(path, &sf);

        if (sf.st_mode & S_IFREG)
            type = SYSAPI_FILE_TYPE_REGFILE;

        if (sf.st_mode & S_IFDIR)
            type = SYSAPI_FILE_TYPE_DIRECT;

        if (type == SYSAPI_FILE_TYPE_REGFILE ||
            type == SYSAPI_FILE_TYPE_DIRECT)
            callback(path, type, app_ctx);
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
        char filedata[300];

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

int sysapi_get_filesize(char *filename)
{
    struct stat st;
    int ret;

    ret = stat(filename, &st);
    if (ret < 0)
        return ret;

    return st.st_size;
}

// mini lsof command implementer API..
int sysapi_get_files_inuse(char *progname,
                           void (*callback)(char *filename, void *app_ctx),
                           void *app_ctx)
{
    DIR *dirp;
    struct dirent *entry;
    char path[300];
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

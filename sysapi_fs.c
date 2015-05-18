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
        if (entry->d_type == DT_REG)
            type = SYSAPI_FILE_TYPE_REGFILE;
        if (entry->d_type == DT_DIR)
            type = SYSAPI_FILE_TYPE_DIRECT;
        if (type == SYSAPI_FILE_TYPE_REGFILE ||
            type == SYSAPI_FILE_TYPE_DIRECT) {
            strncat(path, entry->d_name, strlen(entry->d_name));
            callback(path, type, app_ctx);
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
        char filedata[300];

        int ret = read(fd, filedata, sizeof(filedata));
        if (ret > 0)
            callback(filedata, ret, app_ctx);
        else
            break;
    }

    close(fd);
    return 0;
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


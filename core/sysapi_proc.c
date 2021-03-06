#include "sysapi_proc.h"
#include "sysapi_libgen.h"

#define PROC_MEM_INFO "/proc/meminfo"
#define PROC_CMD_LINE "/proc/cmdline"
#define PROC_CMD_LINE_LEN 100

int sysapi_get_kernel_cmdline(char *cmdline)
{
    int fd;

    fd = open(PROC_CMD_LINE, O_RDONLY);
    if (fd < 0)
        return -1;

    read(fd, cmdline, PROC_CMD_LINE_LEN);

    close(fd);

    return 0;
}

#define PROC_HOSTNAME "/proc/sys/kernel/hostname"

int sysapi_proc_get_hostname(char *hostname, int len)
{
    int fd;

    fd = open(PROC_HOSTNAME, O_RDONLY);
    if (fd < 0)
        return -1;

    read(fd, hostname, len);

    close(fd);

    return 0;
}

int sysapi_proc_set_hostname(char *hostname, int len)
{
    int fd;
    char *buff;

    fd = open(PROC_HOSTNAME, O_WRONLY);
    if (fd < 0)
        return -1;

    buff = calloc(1, len + 1); // for '\n'
    if (!buff) {
        close(fd);
        return -1;
    }

    sprintf(buff, "%s\n", hostname);
    write(fd, buff, len + 1);
    close(fd);

    return 0;
}
#undef PROC_HOSTNAME

#define PROC_OS_RELEASE "/proc/sys/kernel/osrelease"

int sysapi_get_kernel_release(char *release, int len)
{
    int fd;

    fd = open(PROC_OS_RELEASE, O_RDONLY);
    if (fd < 0)
        return -1;

    read(fd, release, len);
    close(fd);

    return 0;
}

#undef PROC_OS_RELEASE

int sysapi_get_kernel_meminfo(struct sysapi_kernel_meminfo *meminfo)
{
#define SYSAPI_MEMTOTAL "MemTotal"
#define SYSAPI_MEMFREE "MemFree"
#define SYSAPI_MEMAVAIL "MemAvailable"
#define SYSAPI_BUFFERS "Buffers"
#define SYSAPI_CACHED "Cached"
#define SYSAPI_SWAP_CACHED "SwapCached"
#define SYSAPI_ACTIVE "Active"
#define SYSAPI_INACTIVE "Inactive"
#define SYSAPI_ACTIVE_ANON "Active(anon)"
#define SYSAPI_INACTIVE_ANON "Inactive(anon)"
#define SYSAPI_ACTIVE_FILE "Active(file)"
#define SYSAPI_INACTIVE_FILE "Inactive(file)"
#define SYSAPI_UNEVICTABLE "Unevictable"
#define SYSAPI_MLOCKED "Mlocked"
#define SYSAPI_SWAP_TOTAL "SwapTotal"
#define SYSAPI_SWAP_FREE "SwapFree"
#define SYSAPI_DIRTY "Dirty"
#define SYSAPI_WRITEBACK "Writeback"
#define SYSAPI_ANONPAGES "AnonPages"
#define SYSAPI_MAPPED "Mapped"
#define SYSAPI_SHMEM "Shmem"
#define SYSAPI_SLAB "Slab"
#define SYSAPI_SRECLAIMABLE "SReclaimable"
#define SYSAPI_SUNRECLAIM "SUnreclaim"
#define SYSAPI_KERNEL_STACK "KernelStack"
#define SYSAPI_PAGE_TABLE "PageTables"
#define SYSAPI_NFS_UNSTABLE "NFS_Unstable"
#define SYSAPI_BOUNCE "Bounce"
#define SYSAPI_WRITEBACK_TMP "WritebackTmp"
#define SYSAPI_COMMIT_LIMIT "CommitLimit"
#define SYSAPI_COMMITED_AS "Committed_AS"
#define SYSAPI_VMALLOC_TOTAL "VmallocTotal"
#define SYSAPI_VMALLOC_USED "VmallocUsed"
#define SYSAPI_VMALLOC_CHUNK "VmallocChunk"
#define SYSAPI_HARDWARE_CORRUPTED "HardwareCorrupted"
#define SYSAPI_ANON_HUGE_PAGES "AnonHugePages"
#define SYSAPI_CMA_TOTAL "CmaTotal"
#define SYSAPI_CMA_FREE "CmaFree"
#define SYSAPI_HUGE_PAGES_TOTAL "HugePages_Total"
#define SYSAPI_HUGE_PAGES_FREE "HugePages_Free"
#define SYSAPI_HUGE_PAGES_RSVD "HugePages_Rsvd"
#define SYSAPI_HUGE_PAGES_SURP "HugePages_Surp"
#define SYSAPI_HUGE_PAGE_SIZE "Hugepagesize"
#define SYSAPI_DIRECT_MAP4K "DirectMap4k"
#define SYSAPI_DIRECT_MAP2M "DirectMap2M"

    struct _meminfo_meta {
        char *name;
        int off;
    } _meminfo [] = {
        {SYSAPI_MEMTOTAL, offsetof(struct sysapi_kernel_meminfo, memtotal)},
        {SYSAPI_MEMFREE, offsetof(struct sysapi_kernel_meminfo, memfree)},
        {SYSAPI_MEMAVAIL, offsetof(struct sysapi_kernel_meminfo, memavail)},
        {SYSAPI_BUFFERS, offsetof(struct sysapi_kernel_meminfo, buffers)},
        {SYSAPI_CACHED, offsetof(struct sysapi_kernel_meminfo, cached)},
        {SYSAPI_SWAP_CACHED, offsetof(struct sysapi_kernel_meminfo, swap_cached)},
        {SYSAPI_ACTIVE, offsetof(struct sysapi_kernel_meminfo, active)},
        {SYSAPI_INACTIVE, offsetof(struct sysapi_kernel_meminfo, inactive)},
        {SYSAPI_ACTIVE_ANON, offsetof(struct sysapi_kernel_meminfo, active_anon)},
        {SYSAPI_INACTIVE_ANON, offsetof(struct sysapi_kernel_meminfo, inactive_anon)},
        {SYSAPI_ACTIVE_FILE, offsetof(struct sysapi_kernel_meminfo, active_file)},
        {SYSAPI_INACTIVE_FILE, offsetof(struct sysapi_kernel_meminfo, inactive_file)},
        {SYSAPI_UNEVICTABLE, offsetof(struct sysapi_kernel_meminfo, unevictable)},
        {SYSAPI_MLOCKED, offsetof(struct sysapi_kernel_meminfo, mlocked)},
        {SYSAPI_SWAP_TOTAL, offsetof(struct sysapi_kernel_meminfo, swap_total)},
        {SYSAPI_SWAP_FREE, offsetof(struct sysapi_kernel_meminfo, swap_free)},
        {SYSAPI_DIRTY, offsetof(struct sysapi_kernel_meminfo, dirty)},
        {SYSAPI_WRITEBACK, offsetof(struct sysapi_kernel_meminfo, writeback)},
        {SYSAPI_ANONPAGES, offsetof(struct sysapi_kernel_meminfo, anonpages)},
        {SYSAPI_BOUNCE, offsetof(struct sysapi_kernel_meminfo, bounce)},
        {SYSAPI_WRITEBACK_TMP, offsetof(struct sysapi_kernel_meminfo, writebacktmp)},
        {SYSAPI_COMMIT_LIMIT, offsetof(struct sysapi_kernel_meminfo, commitlimit)},
        {SYSAPI_MAPPED, offsetof(struct sysapi_kernel_meminfo, mapped)},
        {SYSAPI_SHMEM, offsetof(struct sysapi_kernel_meminfo, shmem)},
        {SYSAPI_SLAB, offsetof(struct sysapi_kernel_meminfo, slab)},
        {SYSAPI_SRECLAIMABLE, offsetof(struct sysapi_kernel_meminfo, sreclaimable)},
        {SYSAPI_SUNRECLAIM, offsetof(struct sysapi_kernel_meminfo, sunreclaim)},
        {SYSAPI_KERNEL_STACK, offsetof(struct sysapi_kernel_meminfo, kernelstack)},
        {SYSAPI_PAGE_TABLE, offsetof(struct sysapi_kernel_meminfo, pagetables)},
        {SYSAPI_NFS_UNSTABLE, offsetof(struct sysapi_kernel_meminfo, nfs_unstable)},
        {SYSAPI_COMMITED_AS, offsetof(struct sysapi_kernel_meminfo, commited_as)},
        {SYSAPI_VMALLOC_TOTAL, offsetof(struct sysapi_kernel_meminfo, vmalloctotal)},
        {SYSAPI_VMALLOC_USED, offsetof(struct sysapi_kernel_meminfo, vmallocchunk)},
        {SYSAPI_VMALLOC_CHUNK, offsetof(struct sysapi_kernel_meminfo, vmallocchunk)},
        {SYSAPI_HARDWARE_CORRUPTED, offsetof(struct sysapi_kernel_meminfo, hardwarecorrupted)},
        {SYSAPI_ANONPAGES, offsetof(struct sysapi_kernel_meminfo, anonpages)},
        {SYSAPI_CMA_TOTAL, offsetof(struct sysapi_kernel_meminfo, cmatotal)},
        {SYSAPI_CMA_FREE, offsetof(struct sysapi_kernel_meminfo, cmafree)},
        {SYSAPI_HUGE_PAGES_TOTAL, offsetof(struct sysapi_kernel_meminfo, hugepagestotal)},
        {SYSAPI_HUGE_PAGES_FREE, offsetof(struct sysapi_kernel_meminfo, hugepagesfree)},
        {SYSAPI_HUGE_PAGES_RSVD, offsetof(struct sysapi_kernel_meminfo, hugepagesrsvd)},
        {SYSAPI_HUGE_PAGES_SURP, offsetof(struct sysapi_kernel_meminfo, hugepagessurp)},
        {SYSAPI_HUGE_PAGE_SIZE, offsetof(struct sysapi_kernel_meminfo, hugepagesize)},
        {SYSAPI_DIRECT_MAP4K, offsetof(struct sysapi_kernel_meminfo, directmap4k)},
        {SYSAPI_DIRECT_MAP2M, offsetof(struct sysapi_kernel_meminfo, directmap2m)}

    };

    int size = sizeof(_meminfo) / sizeof(_meminfo[0]);
    int i;
    FILE *fp;

    fp = fopen(PROC_MEM_INFO, "r");
    if (!fp)
        return -1;

    char buff[2000];

    memset(buff, 0, sizeof(buff));

    while (fgets(buff, sizeof(buff), fp)) {
        int j;
        int list;


        char src[20], dst[20];

        i = 0;

        while (buff[i] != ':') {
            src[i] = buff[i];
            i++;
        }

        src[i] = '\0';

        i++;

        while (buff[i] == ' ')
            i++;

        if (buff[i] == '\n' || buff[i] == '\0')
            continue;

        j = 0;

        while (buff[i]) {
            if ((buff[i] == '\n') || (buff[i] == '\0'))
                break;

            if (buff[i] != ' ')
                dst[j] = buff[i];
            else
                break;

            i++;

            j++;
        }

        dst[j] = '\0';

        for (list = 0; list < size; list++) {
            if (strcmp(_meminfo[list].name, src) == 0) {
                long *value = (long *)((long)(meminfo) + _meminfo[list].off);

                *value = atoi(dst);
            }
        }
    }

    fclose(fp);

    return 0;
}

int sysapi_get_proc_name(char *name, int len)
{
#define PROC_SELF_COM "/proc/self/comm"
    int ret;
    int fd;

    fd = open(PROC_SELF_COM, O_RDWR);
    if (fd < 0)
        return -1;

    ret = read(fd, name, len);
    if (ret < 0) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
#undef PROC_SELF_COM
}

int sysapi_is_proc_mounted()
{
    int fd;
    int ret;

    fd = open("/proc", O_RDONLY);
    ret = fd;
    close(fd);

    return ret < 0 ? -1: 0;
}

UNUSED static void _sysapi_parse_crypto_data(char *filebuf, char *dest)
{
    int i = 0, j = 0;

    while (filebuf[i] != ':')
        i++;

    i++;

    while (filebuf[i] == ' ')
        i++;

    while (filebuf[i]) {
        dest[j] = filebuf[i];
        j++;
        i++;
    }

    dest[j] = '\0';
}

#ifdef CONFIG_INCLUDE_BROKEN_IN_LIB
BROKEN UNUSED int sysapi_get_kernel_crypto(struct sysapi_sys_crypto_info *crypto)
{
#define CRYPTO_NAME "name"
#define CRYPTO_DRIVER "driver"
#define CRYPTO_MODULE "module"
#define CRYPTO_PRIORITY "priority"
#define CRYPTO_REFCNT "refcnt"
#define CRYPTO_SELFTEST "selftest"
#define CRYPTO_TYPE "type"
#define CRYPTO_SEEDSIZE "seedsize"
#define CRYPTO_BLOCKSIZE "blocksize"
#define CRYPTO_DIGESTSIZE "digestsize"
#define CRYPRO_MINKEYSIZE "min keysize"
#define CRYPTO_MAXKEYSIZE "max keysize"
#define PROC_CRYPT "/proc/crypto"
    FILE *fp;
    char filedata[1024];

    fp = fopen(PROC_CRYPT, "r");
    if (!fp)
        return -1;

    memset(crypto, 0, sizeof(struct sysapi_sys_crypto_info));

    int old = 1, changed = 0;

    struct sysapi_sys_crypto_info info;

    while (fgets(filedata, sizeof(filedata), fp)) {
        struct sysapi_sys_crypto_info *new = &info;
            
        if (changed != old) {
            if (changed == 2) {
                crypto = new;
                crypto->next = NULL;
            } else {
                struct sysapi_sys_crypto_info *t = crypto;
                struct sysapi_sys_crypto_info *_t;

                while (t->next) {
                    t = t->next;
                }

                _t = calloc(1, sizeof(struct sysapi_sys_crypto_info));
                if (!_t)
                    return -1;
                _t = new;
                _t->next = NULL;
                t->next = _t;
            }
            old = changed;
        }

        printf("Filedata %s\n", filedata);
        if (strstr(filedata, CRYPTO_NAME)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            strcpy(new->name, data);
            new->avail.name = 1;
            changed++;
        } else if (strstr(filedata, CRYPTO_DRIVER)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            strcpy(new->driver, data);
            new->avail.driver = 1;
        } else if (strstr(filedata, CRYPTO_MODULE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            strcpy(new->module, data);
            new->avail.module = 1;
        } else if (strstr(filedata, CRYPTO_PRIORITY)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->priority = atoi(data);
            new->avail.priority = 1;
        } else if (strstr(filedata, CRYPTO_REFCNT)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->refcnt = atoi(data);
            new->avail.refcnt = 1;
        } else if (strstr(filedata, CRYPTO_SELFTEST)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            strcpy(new->selftest, data);
            new->avail.selftest = 1;
        } else if (strstr(filedata, CRYPTO_TYPE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            strcpy(new->type, data);
            new->avail.type = 1;
        } else if (strstr(filedata, CRYPTO_SEEDSIZE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->seedsize = atoi(data);
            new->avail.seedsize = 1;
        } else if (strstr(filedata, CRYPTO_BLOCKSIZE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->blocksize = atoi(data);
            new->avail.blocksize = 1;
        } else if (strstr(filedata, CRYPTO_DIGESTSIZE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->digestsize = atoi(data);
            new->avail.digestsize = 1;
        } else if (strstr(filedata, CRYPRO_MINKEYSIZE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->min_keysize = atoi(data);
            new->avail.min_keysize = 1;
        } else if (strstr(filedata, CRYPTO_MAXKEYSIZE)) {
            char data[20];
            _sysapi_parse_crypto_data(filedata, data);
            new->max_keysize = atoi(data);
            new->avail.max_keysize = 1;
        }
    }

    return 0;
}
#endif


#ifdef CONFIG_SYSAPI_PROC_TEST
int main(void)
{
    char cmdline[200];
    struct sysapi_kernel_meminfo meminfo;
    struct sysapi_sys_crypto_info crypto;
    char hostname[20];
    char release[30];

    int ret;

    sysapi_get_kernel_release(release, sizeof(release));
    printf("release %s\n", release);

    sysapi_proc_get_hostname(hostname, sizeof(hostname));
    strcpy(hostname, "devnaga");
    sysapi_proc_set_hostname(hostname, strlen(hostname) + 1);
    ret = sysapi_get_kernel_cmdline(cmdline);
    ret = sysapi_get_kernel_meminfo(&meminfo);
    ret = sysapi_get_kernel_crypto(&crypto);

    while (1) {
        struct sysapi_sys_crypto_info *t = &crypto;

        printf("crypto name:\n");
        if (t->avail.name)
            printf("\tname : %s\n", t->name);
        if (t->avail.driver)
            printf("\tdriver : %s\n", t->driver);
        if (t->avail.module)
            printf("\tmodule : %s\n", t->module);
        if (t->avail.priority)
            printf("\tpriority : %d\n", t->priority);
        if (t->avail.refcnt)
            printf("\trefcnt : %d\n", t->refcnt);
        if (t->avail.selftest)
            printf("\tselftest : %s\n", t->selftest);
        if (t->avail.type)
            printf("\ttype : %s\n", t->type);
        if (t->avail.seedsize)
            printf("\tseedsize : %d\n", t->seedsize);
        if (t->avail.blocksize)
            printf("\tblocksize : %d\n", t->blocksize);
        if (t->avail.digestsize)
            printf("\tdigestsize : %d\n", t->digestsize);
        if (t->avail.min_keysize)
            printf("\tminkeysize : %d\n", t->min_keysize);
        if (t->avail.max_keysize)
            printf("\tmaxkeysize : %d\n", t->max_keysize);

        if (t->next)
            t = t->next;
        else
            break;
    }
    return ret;
}
#endif


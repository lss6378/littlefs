#include <stdio.h>
#include "lfs.h"
// #include "lfs_util.h"
#include "bd/lfs_filebd.h"



#define FILE_PATH "lfs.img"
#define BLOCK_SIZE  4096
#define BLOCK_COUNT 2

// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

// file-backed block device
lfs_filebd_t bd;
const struct lfs_filebd_config filebd_cfg = {
    .read_size   = 256,
    .prog_size   = 256,
    .erase_size  = BLOCK_SIZE,
    .erase_count = BLOCK_COUNT,
};

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .context = &bd,
    .read  = lfs_filebd_read,
    .prog  = lfs_filebd_prog,
    .erase = lfs_filebd_erase,
    .sync  = lfs_filebd_sync,

    // block device configuration
    .read_size = 256,
    .prog_size = 256,
    .block_size = BLOCK_SIZE,
    .block_count = BLOCK_COUNT,
    .cache_size = 256,
    .lookahead_size = 8,
    .block_cycles = 500,
};

// entry point
int main(void) {
    // create underlying file block device
    int err = lfs_filebd_create(&cfg, FILE_PATH, &filebd_cfg);
    if (err) {
        printf("filebd create failed: %d\n", err);
        return err;
    }

    // mount the filesystem
    err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        printf("lfs mount failed: %d\n", err);

        err = lfs_format(&lfs, &cfg);
        if (err) {
            printf("lfs format failed: %d\n", err);
            return err;
        }
        err = lfs_mount(&lfs, &cfg);
        if (err) {
            printf("lfs mount failed: %d\n", err);
            return err;
        }
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    // lfs_unmount(&lfs);

    // clean up block device
    // lfs_filebd_destroy(&cfg);

    // print the boot count
    printf("boot_count: %d\n", boot_count);

    return 0;
}

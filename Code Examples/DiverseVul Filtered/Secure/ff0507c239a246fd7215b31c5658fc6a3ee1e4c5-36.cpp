static int64_t sector_lun2qemu(int64_t sector, IscsiLun *iscsilun)
{
    return sector * iscsilun->block_size / BDRV_SECTOR_SIZE;
}
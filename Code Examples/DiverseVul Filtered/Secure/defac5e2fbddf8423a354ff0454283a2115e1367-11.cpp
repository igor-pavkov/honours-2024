static int fd_offset(FDrive *drv)
{
    g_assert(fd_sector(drv) < INT_MAX >> BDRV_SECTOR_BITS);
    return fd_sector(drv) << BDRV_SECTOR_BITS;
}
static void fd_empty_seek_hack(FDrive *drv)
{
    drv->last_sect = 0xFF;
    drv->max_track = 0xFF;
}
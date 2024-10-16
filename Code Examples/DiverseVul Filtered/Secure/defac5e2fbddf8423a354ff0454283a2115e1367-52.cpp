static void fd_init(FDrive *drv)
{
    /* Drive */
    drv->perpendicular = 0;
    /* Disk */
    drv->disk = FLOPPY_DRIVE_TYPE_NONE;
    drv->last_sect = 0;
    drv->max_track = 0;
    drv->ro = true;
    drv->media_changed = 1;
}
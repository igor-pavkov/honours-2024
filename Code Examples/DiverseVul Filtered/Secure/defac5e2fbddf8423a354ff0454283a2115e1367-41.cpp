static void pick_drive_type(FDrive *drv)
{
    if (drv->drive != FLOPPY_DRIVE_TYPE_AUTO) {
        return;
    }

    if (pick_geometry(drv) == 0) {
        drv->drive = drv->disk;
    } else {
        drv->drive = get_fallback_drive_type(drv);
    }

    g_assert(drv->drive != FLOPPY_DRIVE_TYPE_AUTO);
}
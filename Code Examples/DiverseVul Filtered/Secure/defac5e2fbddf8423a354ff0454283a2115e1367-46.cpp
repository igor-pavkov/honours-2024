static FloppyDriveType get_fallback_drive_type(FDrive *drv)
{
    return drv->fdctrl->fallback;
}
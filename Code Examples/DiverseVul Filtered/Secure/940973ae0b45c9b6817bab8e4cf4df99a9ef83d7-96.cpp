static bool ide_cd_is_tray_open(void *opaque)
{
    return ((IDEState *)opaque)->tray_open;
}
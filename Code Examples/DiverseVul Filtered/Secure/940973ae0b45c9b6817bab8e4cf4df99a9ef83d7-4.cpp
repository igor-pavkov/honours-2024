static bool ide_cd_is_medium_locked(void *opaque)
{
    return ((IDEState *)opaque)->tray_locked;
}
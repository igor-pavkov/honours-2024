static void fd_change_cb(void *opaque, bool load, Error **errp)
{
    FDrive *drive = opaque;

    if (!load) {
        blk_set_perm(drive->blk, 0, BLK_PERM_ALL, &error_abort);
    } else {
        if (!blkconf_apply_backend_options(drive->conf,
                                           !blk_supports_write_perm(drive->blk),
                                           false, errp)) {
            return;
        }
    }

    drive->media_changed = 1;
    drive->media_validated = false;
    fd_revalidate(drive);
}
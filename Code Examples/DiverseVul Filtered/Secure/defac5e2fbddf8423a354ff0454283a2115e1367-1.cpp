static void floppy_drive_realize(DeviceState *qdev, Error **errp)
{
    FloppyDrive *dev = FLOPPY_DRIVE(qdev);
    FloppyBus *bus = FLOPPY_BUS(qdev->parent_bus);
    FDrive *drive;
    bool read_only;
    int ret;

    if (dev->unit == -1) {
        for (dev->unit = 0; dev->unit < MAX_FD; dev->unit++) {
            drive = get_drv(bus->fdc, dev->unit);
            if (!drive->blk) {
                break;
            }
        }
    }

    if (dev->unit >= MAX_FD) {
        error_setg(errp, "Can't create floppy unit %d, bus supports "
                   "only %d units", dev->unit, MAX_FD);
        return;
    }

    drive = get_drv(bus->fdc, dev->unit);
    if (drive->blk) {
        error_setg(errp, "Floppy unit %d is in use", dev->unit);
        return;
    }

    if (!dev->conf.blk) {
        dev->conf.blk = blk_create_empty_drive();
        ret = blk_attach_dev(dev->conf.blk, qdev);
        assert(ret == 0);

        /* Don't take write permissions on an empty drive to allow attaching a
         * read-only node later */
        read_only = true;
    } else {
        read_only = !blk_bs(dev->conf.blk) ||
                    !blk_supports_write_perm(dev->conf.blk);
    }

    if (!blkconf_blocksizes(&dev->conf, errp)) {
        return;
    }

    if (dev->conf.logical_block_size != 512 ||
        dev->conf.physical_block_size != 512)
    {
        error_setg(errp, "Physical and logical block size must "
                   "be 512 for floppy");
        return;
    }

    /* rerror/werror aren't supported by fdc and therefore not even registered
     * with qdev. So set the defaults manually before they are used in
     * blkconf_apply_backend_options(). */
    dev->conf.rerror = BLOCKDEV_ON_ERROR_AUTO;
    dev->conf.werror = BLOCKDEV_ON_ERROR_AUTO;

    if (!blkconf_apply_backend_options(&dev->conf, read_only, false, errp)) {
        return;
    }

    /* 'enospc' is the default for -drive, 'report' is what blk_new() gives us
     * for empty drives. */
    if (blk_get_on_error(dev->conf.blk, 0) != BLOCKDEV_ON_ERROR_ENOSPC &&
        blk_get_on_error(dev->conf.blk, 0) != BLOCKDEV_ON_ERROR_REPORT) {
        error_setg(errp, "fdc doesn't support drive option werror");
        return;
    }
    if (blk_get_on_error(dev->conf.blk, 1) != BLOCKDEV_ON_ERROR_REPORT) {
        error_setg(errp, "fdc doesn't support drive option rerror");
        return;
    }

    drive->conf = &dev->conf;
    drive->blk = dev->conf.blk;
    drive->fdctrl = bus->fdc;

    fd_init(drive);
    blk_set_dev_ops(drive->blk, &fd_block_ops, drive);

    /* Keep 'type' qdev property and FDrive->drive in sync */
    drive->drive = dev->type;
    pick_drive_type(drive);
    dev->type = drive->drive;

    fd_revalidate(drive);
}
void fdctrl_init_drives(FloppyBus *bus, DriveInfo **fds)
{
    DeviceState *dev;
    int i;

    for (i = 0; i < MAX_FD; i++) {
        if (fds[i]) {
            dev = qdev_new("floppy");
            qdev_prop_set_uint32(dev, "unit", i);
            qdev_prop_set_enum(dev, "drive-type", FLOPPY_DRIVE_TYPE_AUTO);
            qdev_prop_set_drive_err(dev, "drive", blk_by_legacy_dinfo(fds[i]),
                                    &error_fatal);
            qdev_realize_and_unref(dev, &bus->bus, &error_fatal);
        }
    }
}
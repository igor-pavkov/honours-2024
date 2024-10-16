void fdctrl_realize_common(DeviceState *dev, FDCtrl *fdctrl, Error **errp)
{
    int i, j;
    FDrive *drive;
    static int command_tables_inited = 0;

    if (fdctrl->fallback == FLOPPY_DRIVE_TYPE_AUTO) {
        error_setg(errp, "Cannot choose a fallback FDrive type of 'auto'");
        return;
    }

    /* Fill 'command_to_handler' lookup table */
    if (!command_tables_inited) {
        command_tables_inited = 1;
        for (i = ARRAY_SIZE(handlers) - 1; i >= 0; i--) {
            for (j = 0; j < sizeof(command_to_handler); j++) {
                if ((j & handlers[i].mask) == handlers[i].value) {
                    command_to_handler[j] = i;
                }
            }
        }
    }

    FLOPPY_DPRINTF("init controller\n");
    fdctrl->fifo = qemu_memalign(512, FD_SECTOR_LEN);
    memset(fdctrl->fifo, 0, FD_SECTOR_LEN);
    fdctrl->fifo_size = 512;
    fdctrl->result_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL,
                                             fdctrl_result_timer, fdctrl);

    fdctrl->version = 0x90; /* Intel 82078 controller */
    fdctrl->config = FD_CONFIG_EIS | FD_CONFIG_EFIFO; /* Implicit seek, polling & FIFO enabled */
    fdctrl->num_floppies = MAX_FD;

    floppy_bus_create(fdctrl, &fdctrl->bus, dev);

    for (i = 0; i < MAX_FD; i++) {
        drive = &fdctrl->drives[i];
        drive->fdctrl = fdctrl;
        fd_init(drive);
        fd_revalidate(drive);
    }
}
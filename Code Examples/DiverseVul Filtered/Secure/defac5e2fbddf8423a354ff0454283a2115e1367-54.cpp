static uint32_t fdctrl_read_data(FDCtrl *fdctrl)
{
    FDrive *cur_drv;
    uint32_t retval = 0;
    uint32_t pos;

    cur_drv = get_cur_drv(fdctrl);
    fdctrl->dsr &= ~FD_DSR_PWRDOWN;
    if (!(fdctrl->msr & FD_MSR_RQM) || !(fdctrl->msr & FD_MSR_DIO)) {
        FLOPPY_DPRINTF("error: controller not ready for reading\n");
        return 0;
    }

    /* If data_len spans multiple sectors, the current position in the FIFO
     * wraps around while fdctrl->data_pos is the real position in the whole
     * request. */
    pos = fdctrl->data_pos;
    pos %= FD_SECTOR_LEN;

    switch (fdctrl->phase) {
    case FD_PHASE_EXECUTION:
        assert(fdctrl->msr & FD_MSR_NONDMA);
        if (pos == 0) {
            if (fdctrl->data_pos != 0)
                if (!fdctrl_seek_to_next_sect(fdctrl, cur_drv)) {
                    FLOPPY_DPRINTF("error seeking to next sector %d\n",
                                   fd_sector(cur_drv));
                    return 0;
                }
            if (blk_pread(cur_drv->blk, fd_offset(cur_drv), fdctrl->fifo,
                          BDRV_SECTOR_SIZE)
                < 0) {
                FLOPPY_DPRINTF("error getting sector %d\n",
                               fd_sector(cur_drv));
                /* Sure, image size is too small... */
                memset(fdctrl->fifo, 0, FD_SECTOR_LEN);
            }
        }

        if (++fdctrl->data_pos == fdctrl->data_len) {
            fdctrl->msr &= ~FD_MSR_RQM;
            fdctrl_stop_transfer(fdctrl, 0x00, 0x00, 0x00);
        }
        break;

    case FD_PHASE_RESULT:
        assert(!(fdctrl->msr & FD_MSR_NONDMA));
        if (++fdctrl->data_pos == fdctrl->data_len) {
            fdctrl->msr &= ~FD_MSR_RQM;
            fdctrl_to_command_phase(fdctrl);
            fdctrl_reset_irq(fdctrl);
        }
        break;

    case FD_PHASE_COMMAND:
    default:
        abort();
    }

    retval = fdctrl->fifo[pos];
    FLOPPY_DPRINTF("data register: 0x%02x\n", retval);

    return retval;
}
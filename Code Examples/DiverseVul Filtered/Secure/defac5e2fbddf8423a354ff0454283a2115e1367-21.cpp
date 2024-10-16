static void fdctrl_write_data(FDCtrl *fdctrl, uint32_t value)
{
    FDrive *cur_drv;
    const FDCtrlCommand *cmd;
    uint32_t pos;

    /* Reset mode */
    if (!(fdctrl->dor & FD_DOR_nRESET)) {
        FLOPPY_DPRINTF("Floppy controller in RESET state !\n");
        return;
    }
    if (!(fdctrl->msr & FD_MSR_RQM) || (fdctrl->msr & FD_MSR_DIO)) {
        FLOPPY_DPRINTF("error: controller not ready for writing\n");
        return;
    }
    fdctrl->dsr &= ~FD_DSR_PWRDOWN;

    FLOPPY_DPRINTF("%s: %02x\n", __func__, value);

    /* If data_len spans multiple sectors, the current position in the FIFO
     * wraps around while fdctrl->data_pos is the real position in the whole
     * request. */
    pos = fdctrl->data_pos++;
    pos %= FD_SECTOR_LEN;
    fdctrl->fifo[pos] = value;

    if (fdctrl->data_pos == fdctrl->data_len) {
        fdctrl->msr &= ~FD_MSR_RQM;
    }

    switch (fdctrl->phase) {
    case FD_PHASE_EXECUTION:
        /* For DMA requests, RQM should be cleared during execution phase, so
         * we would have errored out above. */
        assert(fdctrl->msr & FD_MSR_NONDMA);

        /* FIFO data write */
        if (pos == FD_SECTOR_LEN - 1 ||
            fdctrl->data_pos == fdctrl->data_len) {
            cur_drv = get_cur_drv(fdctrl);
            if (blk_pwrite(cur_drv->blk, fd_offset(cur_drv), fdctrl->fifo,
                           BDRV_SECTOR_SIZE, 0) < 0) {
                FLOPPY_DPRINTF("error writing sector %d\n",
                               fd_sector(cur_drv));
                break;
            }
            if (!fdctrl_seek_to_next_sect(fdctrl, cur_drv)) {
                FLOPPY_DPRINTF("error seeking to next sector %d\n",
                               fd_sector(cur_drv));
                break;
            }
        }

        /* Switch to result phase when done with the transfer */
        if (fdctrl->data_pos == fdctrl->data_len) {
            fdctrl_stop_transfer(fdctrl, 0x00, 0x00, 0x00);
        }
        break;

    case FD_PHASE_COMMAND:
        assert(!(fdctrl->msr & FD_MSR_NONDMA));
        assert(fdctrl->data_pos < FD_SECTOR_LEN);

        if (pos == 0) {
            /* The first byte specifies the command. Now we start reading
             * as many parameters as this command requires. */
            cmd = get_command(value);
            fdctrl->data_len = cmd->parameters + 1;
            if (cmd->parameters) {
                fdctrl->msr |= FD_MSR_RQM;
            }
            fdctrl->msr |= FD_MSR_CMDBUSY;
        }

        if (fdctrl->data_pos == fdctrl->data_len) {
            /* We have all parameters now, execute the command */
            fdctrl->phase = FD_PHASE_EXECUTION;

            if (fdctrl->data_state & FD_STATE_FORMAT) {
                fdctrl_format_sector(fdctrl);
                break;
            }

            cmd = get_command(fdctrl->fifo[0]);
            FLOPPY_DPRINTF("Calling handler for '%s'\n", cmd->name);
            cmd->handler(fdctrl, cmd->direction);
        }
        break;

    case FD_PHASE_RESULT:
    default:
        abort();
    }
}
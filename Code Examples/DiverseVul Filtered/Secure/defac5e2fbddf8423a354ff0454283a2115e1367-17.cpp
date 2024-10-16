static void fdctrl_handle_relative_seek_in(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv;

    SET_CUR_DRV(fdctrl, fdctrl->fifo[1] & FD_DOR_SELMASK);
    cur_drv = get_cur_drv(fdctrl);
    if (fdctrl->fifo[2] + cur_drv->track >= cur_drv->max_track) {
        fd_seek(cur_drv, cur_drv->head, cur_drv->max_track - 1,
                cur_drv->sect, 1);
    } else {
        fd_seek(cur_drv, cur_drv->head,
                cur_drv->track + fdctrl->fifo[2], cur_drv->sect, 1);
    }
    fdctrl_to_command_phase(fdctrl);
    /* Raise Interrupt */
    fdctrl->status0 |= FD_SR0_SEEK;
    fdctrl_raise_irq(fdctrl);
}
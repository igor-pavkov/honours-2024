static void fdctrl_handle_recalibrate(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv;

    SET_CUR_DRV(fdctrl, fdctrl->fifo[1] & FD_DOR_SELMASK);
    cur_drv = get_cur_drv(fdctrl);
    fd_recalibrate(cur_drv);
    fdctrl_to_command_phase(fdctrl);
    /* Raise Interrupt */
    fdctrl->status0 |= FD_SR0_SEEK;
    fdctrl_raise_irq(fdctrl);
}
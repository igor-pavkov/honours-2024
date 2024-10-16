static void fdctrl_handle_seek(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv;

    SET_CUR_DRV(fdctrl, fdctrl->fifo[1] & FD_DOR_SELMASK);
    cur_drv = get_cur_drv(fdctrl);
    fdctrl_to_command_phase(fdctrl);
    /* The seek command just sends step pulses to the drive and doesn't care if
     * there is a medium inserted of if it's banging the head against the drive.
     */
    fd_seek(cur_drv, cur_drv->head, fdctrl->fifo[2], cur_drv->sect, 1);
    /* Raise Interrupt */
    fdctrl->status0 |= FD_SR0_SEEK;
    fdctrl_raise_irq(fdctrl);
}
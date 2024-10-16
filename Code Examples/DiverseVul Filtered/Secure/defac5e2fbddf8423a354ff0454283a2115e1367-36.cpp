static void fdctrl_handle_readid(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv = get_cur_drv(fdctrl);

    cur_drv->head = (fdctrl->fifo[1] >> 2) & 1;
    timer_mod(fdctrl->result_timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) +
             (NANOSECONDS_PER_SECOND / 50));
}
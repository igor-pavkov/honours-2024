static void fdctrl_handle_lock(FDCtrl *fdctrl, int direction)
{
    fdctrl->lock = (fdctrl->fifo[0] & 0x80) ? 1 : 0;
    fdctrl->fifo[0] = fdctrl->lock << 4;
    fdctrl_to_result_phase(fdctrl, 1);
}
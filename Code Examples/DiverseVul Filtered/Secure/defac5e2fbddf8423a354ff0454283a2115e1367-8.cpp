static void fdctrl_handle_powerdown_mode(FDCtrl *fdctrl, int direction)
{
    fdctrl->pwrd = fdctrl->fifo[1];
    fdctrl->fifo[0] = fdctrl->fifo[1];
    fdctrl_to_result_phase(fdctrl, 1);
}
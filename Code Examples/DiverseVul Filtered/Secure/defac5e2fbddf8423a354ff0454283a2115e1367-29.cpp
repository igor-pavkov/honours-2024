static void fdctrl_unimplemented(FDCtrl *fdctrl, int direction)
{
    qemu_log_mask(LOG_UNIMP, "fdc: unimplemented command 0x%02x\n",
                  fdctrl->fifo[0]);
    fdctrl->fifo[0] = FD_SR0_INVCMD;
    fdctrl_to_result_phase(fdctrl, 1);
}
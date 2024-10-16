static void fdctrl_to_result_phase(FDCtrl *fdctrl, int fifo_len)
{
    fdctrl->phase = FD_PHASE_RESULT;
    fdctrl->data_dir = FD_DIR_READ;
    fdctrl->data_len = fifo_len;
    fdctrl->data_pos = 0;
    fdctrl->msr |= FD_MSR_CMDBUSY | FD_MSR_RQM | FD_MSR_DIO;
}
static void fdctrl_handle_sense_interrupt_status(FDCtrl *fdctrl, int direction)
{
    FDrive *cur_drv = get_cur_drv(fdctrl);

    if (fdctrl->reset_sensei > 0) {
        fdctrl->fifo[0] =
            FD_SR0_RDYCHG + FD_RESET_SENSEI_COUNT - fdctrl->reset_sensei;
        fdctrl->reset_sensei--;
    } else if (!(fdctrl->sra & FD_SRA_INTPEND)) {
        fdctrl->fifo[0] = FD_SR0_INVCMD;
        fdctrl_to_result_phase(fdctrl, 1);
        return;
    } else {
        fdctrl->fifo[0] =
                (fdctrl->status0 & ~(FD_SR0_HEAD | FD_SR0_DS1 | FD_SR0_DS0))
                | GET_CUR_DRV(fdctrl);
    }

    fdctrl->fifo[1] = cur_drv->track;
    fdctrl_to_result_phase(fdctrl, 2);
    fdctrl_reset_irq(fdctrl);
    fdctrl->status0 = FD_SR0_RDYCHG;
}
void fdctrl_reset(FDCtrl *fdctrl, int do_irq)
{
    int i;

    FLOPPY_DPRINTF("reset controller\n");
    fdctrl_reset_irq(fdctrl);
    /* Initialise controller */
    fdctrl->sra = 0;
    fdctrl->srb = 0xc0;
    if (!fdctrl->drives[1].blk) {
        fdctrl->sra |= FD_SRA_nDRV2;
    }
    fdctrl->cur_drv = 0;
    fdctrl->dor = FD_DOR_nRESET;
    fdctrl->dor |= (fdctrl->dma_chann != -1) ? FD_DOR_DMAEN : 0;
    fdctrl->msr = FD_MSR_RQM;
    fdctrl->reset_sensei = 0;
    timer_del(fdctrl->result_timer);
    /* FIFO state */
    fdctrl->data_pos = 0;
    fdctrl->data_len = 0;
    fdctrl->data_state = 0;
    fdctrl->data_dir = FD_DIR_WRITE;
    for (i = 0; i < MAX_FD; i++)
        fd_recalibrate(&fdctrl->drives[i]);
    fdctrl_to_command_phase(fdctrl);
    if (do_irq) {
        fdctrl->status0 |= FD_SR0_RDYCHG;
        fdctrl_raise_irq(fdctrl);
        fdctrl->reset_sensei = FD_RESET_SENSEI_COUNT;
    }
}
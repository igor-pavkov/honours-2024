static int reconstruct_phase(FDCtrl *fdctrl)
{
    if (fdctrl->msr & FD_MSR_NONDMA) {
        return FD_PHASE_EXECUTION;
    } else if ((fdctrl->msr & FD_MSR_RQM) == 0) {
        /* qemu 2.3 disabled RQM only during DMA transfers */
        return FD_PHASE_EXECUTION;
    } else if (fdctrl->msr & FD_MSR_DIO) {
        return FD_PHASE_RESULT;
    } else {
        return FD_PHASE_COMMAND;
    }
}
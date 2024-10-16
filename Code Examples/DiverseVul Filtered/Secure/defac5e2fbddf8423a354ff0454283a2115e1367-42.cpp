static bool fdc_phase_needed(void *opaque)
{
    FDCtrl *fdctrl = opaque;

    return reconstruct_phase(fdctrl) != fdctrl->phase;
}
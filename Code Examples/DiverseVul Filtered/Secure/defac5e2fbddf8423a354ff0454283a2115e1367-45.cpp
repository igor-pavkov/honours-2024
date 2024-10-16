static int fdc_pre_load(void *opaque)
{
    FDCtrl *s = opaque;
    s->phase = FD_PHASE_RECONSTRUCT;
    return 0;
}
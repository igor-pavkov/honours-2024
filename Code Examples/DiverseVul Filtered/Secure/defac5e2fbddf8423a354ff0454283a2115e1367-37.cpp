static int fdc_post_load(void *opaque, int version_id)
{
    FDCtrl *s = opaque;

    SET_CUR_DRV(s, s->dor_vmstate & FD_DOR_SELMASK);
    s->dor = s->dor_vmstate & ~FD_DOR_SELMASK;

    if (s->phase == FD_PHASE_RECONSTRUCT) {
        s->phase = reconstruct_phase(s);
    }

    return 0;
}
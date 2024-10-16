static int fdc_pre_save(void *opaque)
{
    FDCtrl *s = opaque;

    s->dor_vmstate = s->dor | GET_CUR_DRV(s);

    return 0;
}
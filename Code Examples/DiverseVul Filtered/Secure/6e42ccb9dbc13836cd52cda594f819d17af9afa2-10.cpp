static void imdct_and_windowing_ld(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    float *in    = sce->coeffs;
    float *out   = sce->ret;
    float *saved = sce->saved;
    float *buf  = ac->buf_mdct;

    // imdct
    ac->mdct.imdct_half(&ac->mdct_ld, buf, in);

    // window overlapping
    if (ics->use_kb_window[1]) {
        // AAC LD uses a low overlap sine window instead of a KBD window
        memcpy(out, saved, 192 * sizeof(float));
        ac->fdsp.vector_fmul_window(out + 192, saved + 192, buf, ff_sine_128, 64);
        memcpy(                     out + 320, buf + 64, 192 * sizeof(float));
    } else {
        ac->fdsp.vector_fmul_window(out, saved, buf, ff_sine_512, 256);
    }

    // buffer update
    memcpy(saved, buf + 256, 256 * sizeof(float));
}
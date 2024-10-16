static void update_ltp(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    float *saved     = sce->saved;
    float *saved_ltp = sce->coeffs;
    const float *lwindow = ics->use_kb_window[0] ? ff_aac_kbd_long_1024 : ff_sine_1024;
    const float *swindow = ics->use_kb_window[0] ? ff_aac_kbd_short_128 : ff_sine_128;
    int i;

    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        memcpy(saved_ltp,       saved, 512 * sizeof(float));
        memset(saved_ltp + 576, 0,     448 * sizeof(float));
        ac->fdsp.vector_fmul_reverse(saved_ltp + 448, ac->buf_mdct + 960,     &swindow[64],      64);
        for (i = 0; i < 64; i++)
            saved_ltp[i + 512] = ac->buf_mdct[1023 - i] * swindow[63 - i];
    } else if (ics->window_sequence[0] == LONG_START_SEQUENCE) {
        memcpy(saved_ltp,       ac->buf_mdct + 512, 448 * sizeof(float));
        memset(saved_ltp + 576, 0,                  448 * sizeof(float));
        ac->fdsp.vector_fmul_reverse(saved_ltp + 448, ac->buf_mdct + 960,     &swindow[64],      64);
        for (i = 0; i < 64; i++)
            saved_ltp[i + 512] = ac->buf_mdct[1023 - i] * swindow[63 - i];
    } else { // LONG_STOP or ONLY_LONG
        ac->fdsp.vector_fmul_reverse(saved_ltp,       ac->buf_mdct + 512,     &lwindow[512],     512);
        for (i = 0; i < 512; i++)
            saved_ltp[i + 512] = ac->buf_mdct[1023 - i] * lwindow[511 - i];
    }

    memcpy(sce->ltp_state,      sce->ltp_state+1024, 1024 * sizeof(*sce->ltp_state));
    memcpy(sce->ltp_state+1024, sce->ret,            1024 * sizeof(*sce->ltp_state));
    memcpy(sce->ltp_state+2048, saved_ltp,           1024 * sizeof(*sce->ltp_state));
}
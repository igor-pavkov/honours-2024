static void apply_ltp(AACContext *ac, SingleChannelElement *sce)
{
    const LongTermPrediction *ltp = &sce->ics.ltp;
    const uint16_t *offsets = sce->ics.swb_offset;
    int i, sfb;

    if (sce->ics.window_sequence[0] != EIGHT_SHORT_SEQUENCE) {
        float *predTime = sce->ret;
        float *predFreq = ac->buf_mdct;
        int16_t num_samples = 2048;

        if (ltp->lag < 1024)
            num_samples = ltp->lag + 1024;
        for (i = 0; i < num_samples; i++)
            predTime[i] = sce->ltp_state[i + 2048 - ltp->lag] * ltp->coef;
        memset(&predTime[i], 0, (2048 - i) * sizeof(float));

        ac->windowing_and_mdct_ltp(ac, predFreq, predTime, &sce->ics);

        if (sce->tns.present)
            ac->apply_tns(predFreq, &sce->tns, &sce->ics, 0);

        for (sfb = 0; sfb < FFMIN(sce->ics.max_sfb, MAX_LTP_LONG_SFB); sfb++)
            if (ltp->used[sfb])
                for (i = offsets[sfb]; i < offsets[sfb + 1]; i++)
                    sce->coeffs[i] += predFreq[i];
    }
}
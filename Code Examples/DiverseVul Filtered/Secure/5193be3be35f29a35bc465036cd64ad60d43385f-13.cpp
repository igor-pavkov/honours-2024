static void tsc2102_audio_rate_update(TSC210xState *s)
{
    const TSC210xRateInfo *rate;

    s->codec.tx_rate = 0;
    s->codec.rx_rate = 0;
    if (s->dac_power & (1 << 15))				/* PWDNC */
        return;

    for (rate = tsc2102_rates; rate->rate; rate ++)
        if (rate->dsor == (s->audio_ctrl1 & 0x3f) &&		/* DACFS */
                        rate->fsref == ((s->audio_ctrl3 >> 13) & 1))/* REFFS */
            break;
    if (!rate->rate) {
        printf("%s: unknown sampling rate configured\n", __FUNCTION__);
        return;
    }

    s->codec.tx_rate = rate->rate;
}
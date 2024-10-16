static void tsc2102_audio_output_update(TSC210xState *s)
{
    int enable;
    struct audsettings fmt;

    if (s->dac_voice[0]) {
        tsc210x_out_flush(s, s->codec.out.len);
        s->codec.out.size = 0;
        AUD_set_active_out(s->dac_voice[0], 0);
        AUD_close_out(&s->card, s->dac_voice[0]);
        s->dac_voice[0] = NULL;
    }
    s->codec.cts = 0;

    enable =
            (~s->dac_power & (1 << 15)) &&			/* PWDNC */
            (~s->dac_power & (1 << 10));			/* DAPWDN */
    if (!enable || !s->codec.tx_rate)
        return;

    /* Force our own sampling rate even in slave DAC mode */
    fmt.endianness = 0;
    fmt.nchannels = 2;
    fmt.freq = s->codec.tx_rate;
    fmt.fmt = AUD_FMT_S16;

    s->dac_voice[0] = AUD_open_out(&s->card, s->dac_voice[0],
                    "tsc2102.sink", s, (void *) tsc210x_audio_out_cb, &fmt);
    if (s->dac_voice[0]) {
        s->codec.cts = 1;
        AUD_set_active_out(s->dac_voice[0], 1);
    }
}
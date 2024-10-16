static void tsc210x_i2s_swallow(TSC210xState *s)
{
    if (s->dac_voice[0])
        tsc210x_out_flush(s, s->codec.out.len);
    else
        s->codec.out.len = 0;
}
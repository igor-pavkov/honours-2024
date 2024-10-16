static inline void tsc210x_out_flush(TSC210xState *s, int len)
{
    uint8_t *data = s->codec.out.fifo + s->codec.out.start;
    uint8_t *end = data + len;

    while (data < end)
        data += AUD_write(s->dac_voice[0], data, end - data) ?: (end - data);

    s->codec.out.len -= len;
    if (s->codec.out.len)
        memmove(s->codec.out.fifo, end, s->codec.out.len);
    s->codec.out.start = 0;
}
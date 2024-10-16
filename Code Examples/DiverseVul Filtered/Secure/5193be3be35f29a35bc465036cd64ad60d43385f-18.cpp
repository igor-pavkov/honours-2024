static void tsc210x_audio_out_cb(TSC210xState *s, int free_b)
{
    if (s->codec.out.len >= free_b) {
        tsc210x_out_flush(s, free_b);
        return;
    }

    s->codec.out.size = MIN(free_b, 16384);
    qemu_irq_raise(s->codec.tx_start);
}
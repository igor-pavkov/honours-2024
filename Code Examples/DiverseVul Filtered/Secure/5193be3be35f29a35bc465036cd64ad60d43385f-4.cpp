static void tsc210x_reset(TSC210xState *s)
{
    s->state = 0;
    s->pin_func = 2;
    s->enabled = 0;
    s->busy = 0;
    s->nextfunction = 0;
    s->ref = 0;
    s->timing = 0;
    s->irq = 0;
    s->dav = 0;

    s->audio_ctrl1 = 0x0000;
    s->audio_ctrl2 = 0x4410;
    s->audio_ctrl3 = 0x0000;
    s->pll[0] = 0x1004;
    s->pll[1] = 0x0000;
    s->pll[2] = 0x1fff;
    s->volume = 0xffff;
    s->dac_power = 0x8540;
    s->softstep = 1;
    s->volume_change = 0;
    s->powerdown = 0;
    s->filter_data[0x00] = 0x6be3;
    s->filter_data[0x01] = 0x9666;
    s->filter_data[0x02] = 0x675d;
    s->filter_data[0x03] = 0x6be3;
    s->filter_data[0x04] = 0x9666;
    s->filter_data[0x05] = 0x675d;
    s->filter_data[0x06] = 0x7d83;
    s->filter_data[0x07] = 0x84ee;
    s->filter_data[0x08] = 0x7d83;
    s->filter_data[0x09] = 0x84ee;
    s->filter_data[0x0a] = 0x6be3;
    s->filter_data[0x0b] = 0x9666;
    s->filter_data[0x0c] = 0x675d;
    s->filter_data[0x0d] = 0x6be3;
    s->filter_data[0x0e] = 0x9666;
    s->filter_data[0x0f] = 0x675d;
    s->filter_data[0x10] = 0x7d83;
    s->filter_data[0x11] = 0x84ee;
    s->filter_data[0x12] = 0x7d83;
    s->filter_data[0x13] = 0x84ee;

    s->i2s_tx_rate = 0;
    s->i2s_rx_rate = 0;

    s->kb.scan = 1;
    s->kb.debounce = 0;
    s->kb.mask = 0x0000;
    s->kb.mode = 3;
    s->kb.intr = 0;

    qemu_set_irq(s->pint, !s->irq);
    qemu_set_irq(s->davint, !s->dav);
    qemu_irq_raise(s->kbint);
}
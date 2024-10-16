static void tsc210x_save(QEMUFile *f, void *opaque)
{
    TSC210xState *s = (TSC210xState *) opaque;
    int64_t now = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    int i;

    qemu_put_be16(f, s->x);
    qemu_put_be16(f, s->y);
    qemu_put_byte(f, s->pressure);

    qemu_put_byte(f, s->state);
    qemu_put_byte(f, s->page);
    qemu_put_byte(f, s->offset);
    qemu_put_byte(f, s->command);

    qemu_put_byte(f, s->irq);
    qemu_put_be16s(f, &s->dav);

    timer_put(f, s->timer);
    qemu_put_byte(f, s->enabled);
    qemu_put_byte(f, s->host_mode);
    qemu_put_byte(f, s->function);
    qemu_put_byte(f, s->nextfunction);
    qemu_put_byte(f, s->precision);
    qemu_put_byte(f, s->nextprecision);
    qemu_put_byte(f, s->filter);
    qemu_put_byte(f, s->pin_func);
    qemu_put_byte(f, s->ref);
    qemu_put_byte(f, s->timing);
    qemu_put_be32(f, s->noise);

    qemu_put_be16s(f, &s->audio_ctrl1);
    qemu_put_be16s(f, &s->audio_ctrl2);
    qemu_put_be16s(f, &s->audio_ctrl3);
    qemu_put_be16s(f, &s->pll[0]);
    qemu_put_be16s(f, &s->pll[1]);
    qemu_put_be16s(f, &s->volume);
    qemu_put_sbe64(f, (s->volume_change - now));
    qemu_put_sbe64(f, (s->powerdown - now));
    qemu_put_byte(f, s->softstep);
    qemu_put_be16s(f, &s->dac_power);

    for (i = 0; i < 0x14; i ++)
        qemu_put_be16s(f, &s->filter_data[i]);
}
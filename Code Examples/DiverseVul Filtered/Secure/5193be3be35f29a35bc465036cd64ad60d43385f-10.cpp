uWireSlave *tsc2102_init(qemu_irq pint)
{
    TSC210xState *s;

    s = (TSC210xState *)
            g_malloc0(sizeof(TSC210xState));
    memset(s, 0, sizeof(TSC210xState));
    s->x = 160;
    s->y = 160;
    s->pressure = 0;
    s->precision = s->nextprecision = 0;
    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, tsc210x_timer_tick, s);
    s->pint = pint;
    s->model = 0x2102;
    s->name = "tsc2102";

    s->tr[0] = 0;
    s->tr[1] = 1;
    s->tr[2] = 1;
    s->tr[3] = 0;
    s->tr[4] = 1;
    s->tr[5] = 0;
    s->tr[6] = 1;
    s->tr[7] = 0;

    s->chip.opaque = s;
    s->chip.send = (void *) tsc210x_write;
    s->chip.receive = (void *) tsc210x_read;

    s->codec.opaque = s;
    s->codec.tx_swallow = (void *) tsc210x_i2s_swallow;
    s->codec.set_rate = (void *) tsc210x_i2s_set_rate;
    s->codec.in.fifo = s->in_fifo;
    s->codec.out.fifo = s->out_fifo;

    tsc210x_reset(s);

    qemu_add_mouse_event_handler(tsc210x_touchscreen_event, s, 1,
                    "QEMU TSC2102-driven Touchscreen");

    AUD_register_card(s->name, &s->card);

    qemu_register_reset((void *) tsc210x_reset, s);
    register_savevm(NULL, s->name, -1, 0,
                    tsc210x_save, tsc210x_load, s);

    return &s->chip;
}
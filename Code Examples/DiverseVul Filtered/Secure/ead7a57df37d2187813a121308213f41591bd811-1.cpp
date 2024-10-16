static int ssd0323_init(SSISlave *d)
{
    DeviceState *dev = DEVICE(d);
    ssd0323_state *s = FROM_SSI_SLAVE(ssd0323_state, d);

    s->col_end = 63;
    s->row_end = 79;
    s->con = graphic_console_init(dev, 0, &ssd0323_ops, s);
    qemu_console_resize(s->con, 128 * MAGNIFY, 64 * MAGNIFY);

    qdev_init_gpio_in(dev, ssd0323_cd, 1);

    register_savevm(dev, "ssd0323_oled", -1, 1,
                    ssd0323_save, ssd0323_load, s);
    return 0;
}
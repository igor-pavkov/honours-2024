static int pxa2xx_fir_load(QEMUFile *f, void *opaque, int version_id)
{
    PXA2xxFIrState *s = (PXA2xxFIrState *) opaque;
    int i;

    s->enable = qemu_get_be32(f);

    qemu_get_8s(f, &s->control[0]);
    qemu_get_8s(f, &s->control[1]);
    qemu_get_8s(f, &s->control[2]);
    qemu_get_8s(f, &s->status[0]);
    qemu_get_8s(f, &s->status[1]);

    s->rx_len = qemu_get_byte(f);
    s->rx_start = 0;
    for (i = 0; i < s->rx_len; i ++)
        s->rx_fifo[i] = qemu_get_byte(f);

    return 0;
}
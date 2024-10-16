static void pxa2xx_fir_save(QEMUFile *f, void *opaque)
{
    PXA2xxFIrState *s = (PXA2xxFIrState *) opaque;
    int i;

    qemu_put_be32(f, s->enable);

    qemu_put_8s(f, &s->control[0]);
    qemu_put_8s(f, &s->control[1]);
    qemu_put_8s(f, &s->control[2]);
    qemu_put_8s(f, &s->status[0]);
    qemu_put_8s(f, &s->status[1]);

    qemu_put_byte(f, s->rx_len);
    for (i = 0; i < s->rx_len; i ++)
        qemu_put_byte(f, s->rx_fifo[(s->rx_start + i) & 63]);
}
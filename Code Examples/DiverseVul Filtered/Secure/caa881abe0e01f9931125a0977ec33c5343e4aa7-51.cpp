static void pxa2xx_ssp_save(QEMUFile *f, void *opaque)
{
    PXA2xxSSPState *s = (PXA2xxSSPState *) opaque;
    int i;

    qemu_put_be32(f, s->enable);

    qemu_put_be32s(f, &s->sscr[0]);
    qemu_put_be32s(f, &s->sscr[1]);
    qemu_put_be32s(f, &s->sspsp);
    qemu_put_be32s(f, &s->ssto);
    qemu_put_be32s(f, &s->ssitr);
    qemu_put_be32s(f, &s->sssr);
    qemu_put_8s(f, &s->sstsa);
    qemu_put_8s(f, &s->ssrsa);
    qemu_put_8s(f, &s->ssacd);

    qemu_put_byte(f, s->rx_level);
    for (i = 0; i < s->rx_level; i ++)
        qemu_put_byte(f, s->rx_fifo[(s->rx_start + i) & 0xf]);
}
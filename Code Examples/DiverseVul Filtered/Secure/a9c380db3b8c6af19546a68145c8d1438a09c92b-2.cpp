static void ssi_sd_save(QEMUFile *f, void *opaque)
{
    SSISlave *ss = SSI_SLAVE(opaque);
    ssi_sd_state *s = (ssi_sd_state *)opaque;
    int i;

    qemu_put_be32(f, s->mode);
    qemu_put_be32(f, s->cmd);
    for (i = 0; i < 4; i++)
        qemu_put_be32(f, s->cmdarg[i]);
    for (i = 0; i < 5; i++)
        qemu_put_be32(f, s->response[i]);
    qemu_put_be32(f, s->arglen);
    qemu_put_be32(f, s->response_pos);
    qemu_put_be32(f, s->stopping);

    qemu_put_be32(f, ss->cs);
}
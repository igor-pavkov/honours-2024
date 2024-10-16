static void ssd0323_save(QEMUFile *f, void *opaque)
{
    SSISlave *ss = SSI_SLAVE(opaque);
    ssd0323_state *s = (ssd0323_state *)opaque;
    int i;

    qemu_put_be32(f, s->cmd_len);
    qemu_put_be32(f, s->cmd);
    for (i = 0; i < 8; i++)
        qemu_put_be32(f, s->cmd_data[i]);
    qemu_put_be32(f, s->row);
    qemu_put_be32(f, s->row_start);
    qemu_put_be32(f, s->row_end);
    qemu_put_be32(f, s->col);
    qemu_put_be32(f, s->col_start);
    qemu_put_be32(f, s->col_end);
    qemu_put_be32(f, s->redraw);
    qemu_put_be32(f, s->remap);
    qemu_put_be32(f, s->mode);
    qemu_put_buffer(f, s->framebuffer, sizeof(s->framebuffer));

    qemu_put_be32(f, ss->cs);
}
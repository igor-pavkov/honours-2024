static int ssd0323_load(QEMUFile *f, void *opaque, int version_id)
{
    SSISlave *ss = SSI_SLAVE(opaque);
    ssd0323_state *s = (ssd0323_state *)opaque;
    int i;

    if (version_id != 1)
        return -EINVAL;

    s->cmd_len = qemu_get_be32(f);
    if (s->cmd_len < 0 || s->cmd_len > ARRAY_SIZE(s->cmd_data)) {
        return -EINVAL;
    }
    s->cmd = qemu_get_be32(f);
    for (i = 0; i < 8; i++)
        s->cmd_data[i] = qemu_get_be32(f);
    s->row = qemu_get_be32(f);
    if (s->row < 0 || s->row >= 80) {
        return -EINVAL;
    }
    s->row_start = qemu_get_be32(f);
    if (s->row_start < 0 || s->row_start >= 80) {
        return -EINVAL;
    }
    s->row_end = qemu_get_be32(f);
    if (s->row_end < 0 || s->row_end >= 80) {
        return -EINVAL;
    }
    s->col = qemu_get_be32(f);
    if (s->col < 0 || s->col >= 64) {
        return -EINVAL;
    }
    s->col_start = qemu_get_be32(f);
    if (s->col_start < 0 || s->col_start >= 64) {
        return -EINVAL;
    }
    s->col_end = qemu_get_be32(f);
    if (s->col_end < 0 || s->col_end >= 64) {
        return -EINVAL;
    }
    s->redraw = qemu_get_be32(f);
    s->remap = qemu_get_be32(f);
    s->mode = qemu_get_be32(f);
    if (s->mode != SSD0323_CMD && s->mode != SSD0323_DATA) {
        return -EINVAL;
    }
    qemu_get_buffer(f, s->framebuffer, sizeof(s->framebuffer));

    ss->cs = qemu_get_be32(f);

    return 0;
}
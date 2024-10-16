static int ssi_sd_load(QEMUFile *f, void *opaque, int version_id)
{
    SSISlave *ss = SSI_SLAVE(opaque);
    ssi_sd_state *s = (ssi_sd_state *)opaque;
    int i;

    if (version_id != 1)
        return -EINVAL;

    s->mode = qemu_get_be32(f);
    s->cmd = qemu_get_be32(f);
    for (i = 0; i < 4; i++)
        s->cmdarg[i] = qemu_get_be32(f);
    for (i = 0; i < 5; i++)
        s->response[i] = qemu_get_be32(f);
    s->arglen = qemu_get_be32(f);
    if (s->mode == SSI_SD_CMDARG &&
        (s->arglen < 0 || s->arglen >= ARRAY_SIZE(s->cmdarg))) {
        return -EINVAL;
    }
    s->response_pos = qemu_get_be32(f);
    s->stopping = qemu_get_be32(f);
    if (s->mode == SSI_SD_RESPONSE &&
        (s->response_pos < 0 || s->response_pos >= ARRAY_SIZE(s->response) ||
        (!s->stopping && s->arglen > ARRAY_SIZE(s->response)))) {
        return -EINVAL;
    }

    ss->cs = qemu_get_be32(f);

    return 0;
}
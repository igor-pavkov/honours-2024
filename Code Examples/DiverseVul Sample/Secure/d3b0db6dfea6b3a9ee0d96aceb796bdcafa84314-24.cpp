static ssize_t vnc_client_write_plain(VncState *vs)
{
    ssize_t ret;

#ifdef CONFIG_VNC_SASL
    VNC_DEBUG("Write Plain: Pending output %p size %zd offset %zd. Wait SSF %d\n",
              vs->output.buffer, vs->output.capacity, vs->output.offset,
              vs->sasl.waitWriteSSF);

    if (vs->sasl.conn &&
        vs->sasl.runSSF &&
        vs->sasl.waitWriteSSF) {
        ret = vnc_client_write_buf(vs, vs->output.buffer, vs->sasl.waitWriteSSF);
        if (ret)
            vs->sasl.waitWriteSSF -= ret;
    } else
#endif /* CONFIG_VNC_SASL */
        ret = vnc_client_write_buf(vs, vs->output.buffer, vs->output.offset);
    if (!ret)
        return 0;

    buffer_advance(&vs->output, ret);

    if (vs->output.offset == 0) {
        if (vs->ioc_tag) {
            g_source_remove(vs->ioc_tag);
        }
        vs->ioc_tag = qio_channel_add_watch(
            vs->ioc, G_IO_IN, vnc_client_io, vs, NULL);
    }

    return ret;
}
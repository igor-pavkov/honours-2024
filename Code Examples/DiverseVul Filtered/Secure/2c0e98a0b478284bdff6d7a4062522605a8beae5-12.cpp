static int get_sidx_size(AVFormatContext *s)
{
    int ret;
    AVIOContext *buf;
    MOVMuxContext *mov = s->priv_data;

    if ((ret = ffio_open_null_buf(&buf)) < 0)
        return ret;
    mov_write_sidx_tags(buf, mov, -1, 0);
    return ffio_close_null_buf(buf);
}
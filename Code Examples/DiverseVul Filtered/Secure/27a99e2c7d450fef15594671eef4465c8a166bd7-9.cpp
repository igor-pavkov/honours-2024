static int viv_read_close(AVFormatContext *s)
{
    VividasDemuxContext *viv = s->priv_data;

    av_freep(&viv->sb_pb);
    av_freep(&viv->sb_buf);
    av_freep(&viv->sb_blocks);
    av_freep(&viv->sb_entries);

    return 0;
}
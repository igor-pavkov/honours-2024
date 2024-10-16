static void load_sb_block(AVFormatContext *s, VividasDemuxContext *viv, unsigned expected_size)
{
    uint32_t size = 0;
    int i;
    AVIOContext *pb = 0;

    if (viv->sb_pb) {
        av_free(viv->sb_pb);
        viv->sb_pb = NULL;
    }

    if (viv->sb_buf)
        av_free(viv->sb_buf);

    viv->sb_buf = read_sb_block(s->pb, &size, &viv->sb_key, expected_size);
    if (!viv->sb_buf) {
        return;
    }

    pb = avio_alloc_context(viv->sb_buf, size, 0, NULL, NULL, NULL, NULL);
    if (!pb)
        return;

    viv->sb_pb = pb;

    avio_r8(pb); //  'S'
    avio_r8(pb); //  'B'
    ffio_read_varlen(pb); //  size
    avio_r8(pb); //  junk
    ffio_read_varlen(pb); // first packet

    viv->n_sb_entries = viv->sb_blocks[viv->current_sb].n_packets;

    for (i = 0; i < viv->n_sb_entries; i++) {
        viv->sb_entries[i].size = ffio_read_varlen(pb);
        viv->sb_entries[i].flag = avio_r8(pb);
    }

    ffio_read_varlen(pb);
    avio_r8(pb);

    viv->current_sb_entry = 0;
}
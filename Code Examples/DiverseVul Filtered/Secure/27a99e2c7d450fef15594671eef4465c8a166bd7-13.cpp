static int track_index(VividasDemuxContext *viv, AVFormatContext *s, uint8_t *buf, unsigned size)
{
    int64_t off;
    int64_t poff;
    int maxnp=0;
    AVIOContext pb0, *pb = &pb0;
    int i;
    int64_t filesize = avio_size(s->pb);
    uint64_t n_sb_blocks_tmp;

    ffio_init_context(pb, buf, size, 0, NULL, NULL, NULL, NULL);

    ffio_read_varlen(pb); // track_index_len
    avio_r8(pb); // 'c'
    n_sb_blocks_tmp = ffio_read_varlen(pb);
    if (n_sb_blocks_tmp > size / 2)
        return AVERROR_INVALIDDATA;
    viv->sb_blocks = av_calloc(n_sb_blocks_tmp, sizeof(*viv->sb_blocks));
    if (!viv->sb_blocks) {
        return AVERROR(ENOMEM);
    }
    viv->n_sb_blocks = n_sb_blocks_tmp;

    off = 0;
    poff = 0;

    for (i = 0; i < viv->n_sb_blocks; i++) {
        uint64_t size_tmp      = ffio_read_varlen(pb);
        uint64_t n_packets_tmp = ffio_read_varlen(pb);

        if (size_tmp > INT_MAX || n_packets_tmp > INT_MAX)
            return AVERROR_INVALIDDATA;

        viv->sb_blocks[i].byte_offset = off;
        viv->sb_blocks[i].packet_offset = poff;

        viv->sb_blocks[i].size = size_tmp;
        viv->sb_blocks[i].n_packets = n_packets_tmp;

        off += viv->sb_blocks[i].size;
        poff += viv->sb_blocks[i].n_packets;

        if (maxnp < viv->sb_blocks[i].n_packets)
            maxnp = viv->sb_blocks[i].n_packets;
    }

    if (filesize > 0 && poff > filesize)
        return AVERROR_INVALIDDATA;

    viv->sb_entries = av_calloc(maxnp, sizeof(VIV_SB_entry));
    if (!viv->sb_entries)
        return AVERROR(ENOMEM);

    return 0;
}
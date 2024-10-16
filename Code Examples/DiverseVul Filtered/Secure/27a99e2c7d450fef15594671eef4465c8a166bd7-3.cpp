static int viv_read_header(AVFormatContext *s)
{
    VividasDemuxContext *viv = s->priv_data;
    AVIOContext *pb = s->pb;
    int64_t header_end;
    int num_tracks;
    uint32_t key, k2;
    uint32_t v;
    uint8_t keybuffer[187];
    uint32_t b22_size = 0;
    uint32_t b22_key = 0;
    uint8_t *buf = 0;
    int ret;

    avio_skip(pb, 9);

    header_end = avio_tell(pb);

    header_end += ffio_read_varlen(pb);

    num_tracks = avio_r8(pb);

    if (num_tracks != 1) {
        av_log(s, AV_LOG_ERROR, "number of tracks %d is not 1\n", num_tracks);
        return AVERROR(EINVAL);
    }

    v = avio_r8(pb);
    avio_seek(pb, v, SEEK_CUR);

    avio_read(pb, keybuffer, 187);
    key = decode_key(keybuffer);
    viv->sb_key = key;

    avio_rl32(pb);

    for (;;) {
        int64_t here = avio_tell(pb);
        int block_len, block_type;

        if (here >= header_end)
            break;

        block_len = ffio_read_varlen(pb);
        if (avio_feof(pb) || block_len <= 0)
            return AVERROR_INVALIDDATA;

        block_type = avio_r8(pb);

        if (block_type == 22) {
            avio_read(pb, keybuffer, 187);
            b22_key = decode_key(keybuffer);
            b22_size = avio_rl32(pb);
        }

        avio_seek(pb, here + block_len, SEEK_SET);
    }

    if (b22_size) {
        k2 = b22_key;
        buf = read_vblock(pb, &v, b22_key, &k2, 0);
        if (!buf)
            return AVERROR(EIO);

        av_free(buf);
    }

    k2 = key;
    buf = read_vblock(pb, &v, key, &k2, 0);
    if (!buf)
        return AVERROR(EIO);
    ret = track_header(viv, s, buf, v);
    av_free(buf);
    if (ret < 0)
        return ret;

    buf = read_vblock(pb, &v, key, &k2, v);
    if (!buf)
        return AVERROR(EIO);
    ret = track_index(viv, s, buf, v);
    av_free(buf);
    if (ret < 0)
        goto fail;

    viv->sb_offset = avio_tell(pb);
    if (viv->n_sb_blocks > 0) {
        viv->current_sb = 0;
        load_sb_block(s, viv, viv->sb_blocks[0].size);
    } else {
        viv->current_sb = -1;
    }

    return 0;
fail:
    av_freep(&viv->sb_blocks);
    return ret;
}
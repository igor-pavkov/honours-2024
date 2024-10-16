static int mov_read_kind(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVFormatContext *ctx = c->fc;
    AVStream *st = NULL;
    AVBPrint scheme_buf, value_buf;
    int64_t scheme_str_len = 0, value_str_len = 0;
    int version, flags, ret = AVERROR_BUG;
    int64_t size = atom.size;

    if (atom.size < 6)
        // 4 bytes for version + flags, 2x 1 byte for null
        return AVERROR_INVALIDDATA;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    version = avio_r8(pb);
    flags   = avio_rb24(pb);
    size   -= 4;

    if (version != 0 || flags != 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Unsupported 'kind' box with version %d, flags: %x",
               version, flags);
        return AVERROR_INVALIDDATA;
    }

    av_bprint_init(&scheme_buf, 0, AV_BPRINT_SIZE_UNLIMITED);
    av_bprint_init(&value_buf,  0, AV_BPRINT_SIZE_UNLIMITED);

    if ((scheme_str_len = ff_read_string_to_bprint_overwrite(pb, &scheme_buf,
                                                             size)) < 0) {
        ret = scheme_str_len;
        goto cleanup;
    }

    if (scheme_str_len + 1 >= size) {
        // we need to have another string, even if nullptr.
        // we check with + 1 since we expect that if size was not hit,
        // an additional null was read.
        ret = AVERROR_INVALIDDATA;
        goto cleanup;
    }

    size -= scheme_str_len + 1;

    if ((value_str_len = ff_read_string_to_bprint_overwrite(pb, &value_buf,
                                                            size)) < 0) {
        ret = value_str_len;
        goto cleanup;
    }

    if (value_str_len == size) {
        // in case of no trailing null, box is not valid.
        ret = AVERROR_INVALIDDATA;
        goto cleanup;
    }

    av_log(ctx, AV_LOG_TRACE,
           "%s stream %d KindBox(scheme: %s, value: %s)\n",
           av_get_media_type_string(st->codecpar->codec_type),
           st->index,
           scheme_buf.str, value_buf.str);

    for (int i = 0; ff_mov_track_kind_table[i].scheme_uri; i++) {
        const struct MP4TrackKindMapping map = ff_mov_track_kind_table[i];
        if (!av_strstart(scheme_buf.str, map.scheme_uri, NULL))
            continue;

        for (int j = 0; map.value_maps[j].disposition; j++) {
            const struct MP4TrackKindValueMapping value_map = map.value_maps[j];
            if (!av_strstart(value_buf.str, value_map.value, NULL))
                continue;

            st->disposition |= value_map.disposition;
        }
    }

    ret = 0;

cleanup:

    av_bprint_finalize(&scheme_buf, NULL);
    av_bprint_finalize(&value_buf,  NULL);

    return ret;
}
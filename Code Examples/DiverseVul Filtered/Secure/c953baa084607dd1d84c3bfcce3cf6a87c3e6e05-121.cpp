static int mov_read_sgpd(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    uint8_t version;
    uint32_t grouping_type;
    uint32_t default_length;
    av_unused uint32_t default_group_description_index;
    uint32_t entry_count;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams - 1];
    sc = st->priv_data;

    version = avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */
    grouping_type = avio_rl32(pb);

    /*
     * This function only supports "sync" boxes, but the code is able to parse
     * other boxes (such as "tscl", "tsas" and "stsa")
     */
    if (grouping_type != MKTAG('s','y','n','c'))
        return 0;

    default_length = version >= 1 ? avio_rb32(pb) : 0;
    default_group_description_index = version >= 2 ? avio_rb32(pb) : 0;
    entry_count = avio_rb32(pb);

    av_freep(&sc->sgpd_sync);
    sc->sgpd_sync_count = entry_count;
    sc->sgpd_sync = av_calloc(entry_count, sizeof(*sc->sgpd_sync));
    if (!sc->sgpd_sync)
        return AVERROR(ENOMEM);

    for (uint32_t i = 0; i < entry_count && !pb->eof_reached; i++) {
        uint32_t description_length = default_length;
        if (version >= 1 && default_length == 0)
            description_length = avio_rb32(pb);
        if (grouping_type == MKTAG('s','y','n','c')) {
            const uint8_t nal_unit_type = avio_r8(pb) & 0x3f;
            sc->sgpd_sync[i] = nal_unit_type;
            description_length -= 1;
        }
        avio_skip(pb, description_length);
    }

    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_WARNING, "reached eof, corrupted SGPD atom\n");
        return AVERROR_EOF;
    }

    return 0;
}
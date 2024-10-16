static int mov_read_stsd(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    int ret, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams - 1];
    sc = st->priv_data;

    sc->stsd_version = avio_r8(pb);
    avio_rb24(pb); /* flags */
    entries = avio_rb32(pb);

    /* Each entry contains a size (4 bytes) and format (4 bytes). */
    if (entries <= 0 || entries > atom.size / 8 || entries > 1024) {
        av_log(c->fc, AV_LOG_ERROR, "invalid STSD entries %d\n", entries);
        return AVERROR_INVALIDDATA;
    }

    if (sc->extradata) {
        av_log(c->fc, AV_LOG_ERROR,
               "Duplicate stsd found in this track.\n");
        return AVERROR_INVALIDDATA;
    }

    /* Prepare space for hosting multiple extradata. */
    sc->extradata = av_calloc(entries, sizeof(*sc->extradata));
    if (!sc->extradata)
        return AVERROR(ENOMEM);

    sc->extradata_size = av_calloc(entries, sizeof(*sc->extradata_size));
    if (!sc->extradata_size) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    ret = ff_mov_read_stsd_entries(c, pb, entries);
    if (ret < 0)
        goto fail;

    /* Restore back the primary extradata. */
    av_freep(&st->codecpar->extradata);
    st->codecpar->extradata_size = sc->extradata_size[0];
    if (sc->extradata_size[0]) {
        st->codecpar->extradata = av_mallocz(sc->extradata_size[0] + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!st->codecpar->extradata)
            return AVERROR(ENOMEM);
        memcpy(st->codecpar->extradata, sc->extradata[0], sc->extradata_size[0]);
    }

    return mov_finalize_stsd_codec(c, pb, st, sc);
fail:
    if (sc->extradata) {
        int j;
        for (j = 0; j < sc->stsd_count; j++)
            av_freep(&sc->extradata[j]);
    }

    av_freep(&sc->extradata);
    av_freep(&sc->extradata_size);
    return ret;
}
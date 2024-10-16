static int mov_read_stss(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    FFStream *sti;
    MOVStreamContext *sc;
    unsigned int i, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sti = ffstream(st);
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */

    entries = avio_rb32(pb);

    av_log(c->fc, AV_LOG_TRACE, "keyframe_count = %u\n", entries);

    if (!entries) {
        sc->keyframe_absent = 1;
        if (!sti->need_parsing && st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            sti->need_parsing = AVSTREAM_PARSE_HEADERS;
        return 0;
    }
    if (sc->keyframes)
        av_log(c->fc, AV_LOG_WARNING, "Duplicated STSS atom\n");
    if (entries >= UINT_MAX / sizeof(int))
        return AVERROR_INVALIDDATA;
    av_freep(&sc->keyframes);
    sc->keyframe_count = 0;
    sc->keyframes = av_malloc_array(entries, sizeof(*sc->keyframes));
    if (!sc->keyframes)
        return AVERROR(ENOMEM);

    for (i = 0; i < entries && !pb->eof_reached; i++) {
        sc->keyframes[i] = avio_rb32(pb);
    }

    sc->keyframe_count = i;

    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_WARNING, "reached eof, corrupted STSS atom\n");
        return AVERROR_EOF;
    }

    return 0;
}
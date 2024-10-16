static int mov_read_sdtp(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    int64_t i, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams - 1];
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */
    entries = atom.size - 4;

    av_log(c->fc, AV_LOG_TRACE, "track[%u].sdtp.entries = %" PRId64 "\n",
           c->fc->nb_streams - 1, entries);

    if (sc->sdtp_data)
        av_log(c->fc, AV_LOG_WARNING, "Duplicated SDTP atom\n");
    av_freep(&sc->sdtp_data);
    sc->sdtp_count = 0;

    sc->sdtp_data = av_malloc(entries);
    if (!sc->sdtp_data)
        return AVERROR(ENOMEM);

    for (i = 0; i < entries && !pb->eof_reached; i++)
        sc->sdtp_data[i] = avio_r8(pb);
    sc->sdtp_count = i;

    return 0;
}
static int mov_read_stsc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */

    entries = avio_rb32(pb);
    if ((uint64_t)entries * 12 + 4 > atom.size)
        return AVERROR_INVALIDDATA;

    av_log(c->fc, AV_LOG_TRACE, "track[%u].stsc.entries = %u\n", c->fc->nb_streams - 1, entries);

    if (!entries)
        return 0;
    if (sc->stsc_data) {
        av_log(c->fc, AV_LOG_WARNING, "Ignoring duplicated STSC atom\n");
        return 0;
    }
    av_free(sc->stsc_data);
    sc->stsc_count = 0;
    sc->stsc_data = av_malloc_array(entries, sizeof(*sc->stsc_data));
    if (!sc->stsc_data)
        return AVERROR(ENOMEM);

    for (i = 0; i < entries && !pb->eof_reached; i++) {
        sc->stsc_data[i].first = avio_rb32(pb);
        sc->stsc_data[i].count = avio_rb32(pb);
        sc->stsc_data[i].id = avio_rb32(pb);
    }

    sc->stsc_count = i;
    for (i = sc->stsc_count - 1; i < UINT_MAX; i--) {
        int64_t first_min = i + 1;
        if ((i+1 < sc->stsc_count && sc->stsc_data[i].first >= sc->stsc_data[i+1].first) ||
            (i > 0 && sc->stsc_data[i].first <= sc->stsc_data[i-1].first) ||
            sc->stsc_data[i].first < first_min ||
            sc->stsc_data[i].count < 1 ||
            sc->stsc_data[i].id < 1) {
            av_log(c->fc, AV_LOG_WARNING, "STSC entry %d is invalid (first=%d count=%d id=%d)\n", i, sc->stsc_data[i].first, sc->stsc_data[i].count, sc->stsc_data[i].id);
            if (i+1 >= sc->stsc_count) {
                if (sc->stsc_data[i].count == 0 && i > 0) {
                    sc->stsc_count --;
                    continue;
                }
                sc->stsc_data[i].first = FFMAX(sc->stsc_data[i].first, first_min);
                if (i > 0 && sc->stsc_data[i].first <= sc->stsc_data[i-1].first)
                    sc->stsc_data[i].first = FFMIN(sc->stsc_data[i-1].first + 1LL, INT_MAX);
                sc->stsc_data[i].count = FFMAX(sc->stsc_data[i].count, 1);
                sc->stsc_data[i].id    = FFMAX(sc->stsc_data[i].id, 1);
                continue;
            }
            av_assert0(sc->stsc_data[i+1].first >= 2);
            // We replace this entry by the next valid
            sc->stsc_data[i].first = sc->stsc_data[i+1].first - 1;
            sc->stsc_data[i].count = sc->stsc_data[i+1].count;
            sc->stsc_data[i].id    = sc->stsc_data[i+1].id;
        }
    }

    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_WARNING, "reached eof, corrupted STSC atom\n");
        return AVERROR_EOF;
    }

    return 0;
}
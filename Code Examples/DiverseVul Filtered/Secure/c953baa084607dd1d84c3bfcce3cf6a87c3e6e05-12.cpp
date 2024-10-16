static int mov_read_elst(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;
    int i, edit_count, version;
    int64_t elst_entry_size;

    if (c->fc->nb_streams < 1 || c->ignore_editlist)
        return 0;
    sc = c->fc->streams[c->fc->nb_streams-1]->priv_data;

    version = avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */
    edit_count = avio_rb32(pb); /* entries */
    atom.size -= 8;

    elst_entry_size = version == 1 ? 20 : 12;
    if (atom.size != edit_count * elst_entry_size) {
        if (c->fc->strict_std_compliance >= FF_COMPLIANCE_STRICT) {
            av_log(c->fc, AV_LOG_ERROR, "Invalid edit list entry_count: %d for elst atom of size: %"PRId64" bytes.\n",
                   edit_count, atom.size + 8);
            return AVERROR_INVALIDDATA;
        } else {
            edit_count = atom.size / elst_entry_size;
            if (edit_count * elst_entry_size != atom.size) {
                av_log(c->fc, AV_LOG_WARNING, "ELST atom of %"PRId64" bytes, bigger than %d entries.\n", atom.size, edit_count);
            }
        }
    }

    if (!edit_count)
        return 0;
    if (sc->elst_data)
        av_log(c->fc, AV_LOG_WARNING, "Duplicated ELST atom\n");
    av_free(sc->elst_data);
    sc->elst_count = 0;
    sc->elst_data = av_malloc_array(edit_count, sizeof(*sc->elst_data));
    if (!sc->elst_data)
        return AVERROR(ENOMEM);

    av_log(c->fc, AV_LOG_TRACE, "track[%u].edit_count = %i\n", c->fc->nb_streams - 1, edit_count);
    for (i = 0; i < edit_count && atom.size > 0 && !pb->eof_reached; i++) {
        MOVElst *e = &sc->elst_data[i];

        if (version == 1) {
            e->duration = avio_rb64(pb);
            e->time     = avio_rb64(pb);
            atom.size -= 16;
        } else {
            e->duration = avio_rb32(pb); /* segment duration */
            e->time     = (int32_t)avio_rb32(pb); /* media time */
            atom.size -= 8;
        }
        e->rate = avio_rb32(pb) / 65536.0;
        atom.size -= 4;
        av_log(c->fc, AV_LOG_TRACE, "duration=%"PRId64" time=%"PRId64" rate=%f\n",
               e->duration, e->time, e->rate);

        if (e->time < 0 && e->time != -1 &&
            c->fc->strict_std_compliance >= FF_COMPLIANCE_STRICT) {
            av_log(c->fc, AV_LOG_ERROR, "Track %d, edit %d: Invalid edit list media time=%"PRId64"\n",
                   c->fc->nb_streams-1, i, e->time);
            return AVERROR_INVALIDDATA;
        }
    }
    sc->elst_count = i;

    return 0;
}
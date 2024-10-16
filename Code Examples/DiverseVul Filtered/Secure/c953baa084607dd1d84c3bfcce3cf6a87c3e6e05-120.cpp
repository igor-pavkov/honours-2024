static int mov_read_tfdt(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVFragment *frag = &c->fragment;
    AVStream *st = NULL;
    MOVStreamContext *sc;
    int version, i;
    MOVFragmentStreamInfo * frag_stream_info;
    int64_t base_media_decode_time;

    for (i = 0; i < c->fc->nb_streams; i++) {
        if (c->fc->streams[i]->id == frag->track_id) {
            st = c->fc->streams[i];
            break;
        }
    }
    if (!st) {
        av_log(c->fc, AV_LOG_WARNING, "could not find corresponding track id %u\n", frag->track_id);
        return 0;
    }
    sc = st->priv_data;
    if (sc->pseudo_stream_id + 1 != frag->stsd_id && sc->pseudo_stream_id != -1)
        return 0;
    version = avio_r8(pb);
    avio_rb24(pb); /* flags */
    if (version) {
        base_media_decode_time = avio_rb64(pb);
    } else {
        base_media_decode_time = avio_rb32(pb);
    }

    frag_stream_info = get_current_frag_stream_info(&c->frag_index);
    if (frag_stream_info)
        frag_stream_info->tfdt_dts = base_media_decode_time;
    sc->track_end = base_media_decode_time;

    return 0;
}
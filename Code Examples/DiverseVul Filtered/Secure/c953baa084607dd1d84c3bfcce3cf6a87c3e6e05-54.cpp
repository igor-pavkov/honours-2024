static int mov_read_sidx(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int64_t stream_size = avio_size(pb);
    int64_t offset = av_sat_add64(avio_tell(pb), atom.size), pts, timestamp;
    uint8_t version, is_complete;
    int64_t offadd;
    unsigned i, j, track_id, item_count;
    AVStream *st = NULL;
    AVStream *ref_st = NULL;
    MOVStreamContext *sc, *ref_sc = NULL;
    AVRational timescale;

    version = avio_r8(pb);
    if (version > 1) {
        avpriv_request_sample(c->fc, "sidx version %u", version);
        return 0;
    }

    avio_rb24(pb); // flags

    track_id = avio_rb32(pb); // Reference ID
    for (i = 0; i < c->fc->nb_streams; i++) {
        if (c->fc->streams[i]->id == track_id) {
            st = c->fc->streams[i];
            break;
        }
    }
    if (!st) {
        av_log(c->fc, AV_LOG_WARNING, "could not find corresponding track id %d\n", track_id);
        return 0;
    }

    sc = st->priv_data;

    timescale = av_make_q(1, avio_rb32(pb));

    if (timescale.den <= 0) {
        av_log(c->fc, AV_LOG_ERROR, "Invalid sidx timescale 1/%d\n", timescale.den);
        return AVERROR_INVALIDDATA;
    }

    if (version == 0) {
        pts = avio_rb32(pb);
        offadd= avio_rb32(pb);
    } else {
        pts = avio_rb64(pb);
        offadd= avio_rb64(pb);
    }
    if (av_sat_add64(offset, offadd) != offset + (uint64_t)offadd)
        return AVERROR_INVALIDDATA;

    offset += (uint64_t)offadd;

    avio_rb16(pb); // reserved

    item_count = avio_rb16(pb);
    if (item_count == 0)
        return AVERROR_INVALIDDATA;

    for (i = 0; i < item_count; i++) {
        int index;
        MOVFragmentStreamInfo * frag_stream_info;
        uint32_t size = avio_rb32(pb);
        uint32_t duration = avio_rb32(pb);
        if (size & 0x80000000) {
            avpriv_request_sample(c->fc, "sidx reference_type 1");
            return AVERROR_PATCHWELCOME;
        }
        avio_rb32(pb); // sap_flags
        timestamp = av_rescale_q(pts, timescale, st->time_base);

        index = update_frag_index(c, offset);
        frag_stream_info = get_frag_stream_info(&c->frag_index, index, track_id);
        if (frag_stream_info)
            frag_stream_info->sidx_pts = timestamp;

        if (av_sat_add64(offset, size) != offset + (uint64_t)size ||
            av_sat_add64(pts, duration) != pts + (uint64_t)duration
        )
            return AVERROR_INVALIDDATA;
        offset += size;
        pts += duration;
    }

    st->duration = sc->track_end = pts;

    sc->has_sidx = 1;

    // See if the remaining bytes are just an mfra which we can ignore.
    is_complete = offset == stream_size;
    if (!is_complete && (pb->seekable & AVIO_SEEKABLE_NORMAL) && stream_size > 0 ) {
        int64_t ret;
        int64_t original_pos = avio_tell(pb);
        if (!c->have_read_mfra_size) {
            if ((ret = avio_seek(pb, stream_size - 4, SEEK_SET)) < 0)
                return ret;
            c->mfra_size = avio_rb32(pb);
            c->have_read_mfra_size = 1;
            if ((ret = avio_seek(pb, original_pos, SEEK_SET)) < 0)
                return ret;
        }
        if (offset == stream_size - c->mfra_size)
            is_complete = 1;
    }

    if (is_complete) {
        // Find first entry in fragment index that came from an sidx.
        // This will pretty much always be the first entry.
        for (i = 0; i < c->frag_index.nb_items; i++) {
            MOVFragmentIndexItem * item = &c->frag_index.item[i];
            for (j = 0; ref_st == NULL && j < item->nb_stream_info; j++) {
                MOVFragmentStreamInfo * si;
                si = &item->stream_info[j];
                if (si->sidx_pts != AV_NOPTS_VALUE) {
                    ref_st = c->fc->streams[j];
                    ref_sc = ref_st->priv_data;
                    break;
                }
            }
        }
        if (ref_st) for (i = 0; i < c->fc->nb_streams; i++) {
            st = c->fc->streams[i];
            sc = st->priv_data;
            if (!sc->has_sidx) {
                st->duration = sc->track_end = av_rescale(ref_st->duration, sc->time_scale, ref_sc->time_scale);
            }
        }

        c->frag_index.complete = 1;
    }

    return 0;
}
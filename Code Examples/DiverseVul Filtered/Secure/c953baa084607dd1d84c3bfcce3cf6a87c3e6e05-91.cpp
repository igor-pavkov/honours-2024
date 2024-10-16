static int mov_read_stts(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries, alloc_size = 0;
    int64_t duration = 0;
    int64_t total_sample_count = 0;
    int64_t current_dts = 0;
    int64_t corrected_dts = 0;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */
    entries = avio_rb32(pb);

    av_log(c->fc, AV_LOG_TRACE, "track[%u].stts.entries = %u\n",
            c->fc->nb_streams-1, entries);

    if (sc->stts_data)
        av_log(c->fc, AV_LOG_WARNING, "Duplicated STTS atom\n");
    av_freep(&sc->stts_data);
    sc->stts_count = 0;
    if (entries >= INT_MAX / sizeof(*sc->stts_data))
        return AVERROR(ENOMEM);

    for (i = 0; i < entries && !pb->eof_reached; i++) {
        unsigned int sample_duration;
        unsigned int sample_count;
        unsigned int min_entries = FFMIN(FFMAX(i + 1, 1024 * 1024), entries);
        MOVStts *stts_data = av_fast_realloc(sc->stts_data, &alloc_size,
                                             min_entries * sizeof(*sc->stts_data));
        if (!stts_data) {
            av_freep(&sc->stts_data);
            sc->stts_count = 0;
            return AVERROR(ENOMEM);
        }
        sc->stts_count = min_entries;
        sc->stts_data = stts_data;

        sample_count    = avio_rb32(pb);
        sample_duration = avio_rb32(pb);

        sc->stts_data[i].count= sample_count;
        sc->stts_data[i].duration= sample_duration;

        av_log(c->fc, AV_LOG_TRACE, "sample_count=%u, sample_duration=%u\n",
                sample_count, sample_duration);

        /* STTS sample offsets are uint32 but some files store it as int32
         * with negative values used to correct DTS delays.
           There may be abnormally large values as well. */
        if (sample_duration > c->max_stts_delta) {
            // assume high delta is a correction if negative when cast as int32
            int32_t delta_magnitude = (int32_t)sample_duration;
            av_log(c->fc, AV_LOG_WARNING, "Too large sample offset %u in stts entry %u with count %u in st:%d. Clipping to 1.\n",
                   sample_duration, i, sample_count, st->index);
            sc->stts_data[i].duration = 1;
            corrected_dts += (delta_magnitude < 0 ? (int64_t)delta_magnitude : 1) * sample_count;
        } else {
            corrected_dts += sample_duration * sample_count;
        }

        current_dts += sc->stts_data[i].duration * sample_count;

        if (current_dts > corrected_dts) {
            int64_t drift = (current_dts - corrected_dts)/FFMAX(sample_count, 1);
            uint32_t correction = (sc->stts_data[i].duration > drift) ? drift : sc->stts_data[i].duration - 1;
            current_dts -= correction * sample_count;
            sc->stts_data[i].duration -= correction;
        }

        duration+=(int64_t)sc->stts_data[i].duration*(uint64_t)sc->stts_data[i].count;
        total_sample_count+=sc->stts_data[i].count;
    }

    sc->stts_count = i;

    if (duration > 0 &&
        duration <= INT64_MAX - sc->duration_for_fps &&
        total_sample_count <= INT_MAX - sc->nb_frames_for_fps) {
        sc->duration_for_fps  += duration;
        sc->nb_frames_for_fps += total_sample_count;
    }

    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_WARNING, "reached eof, corrupted STTS atom\n");
        return AVERROR_EOF;
    }

    st->nb_frames= total_sample_count;
    if (duration)
        st->duration= FFMIN(st->duration, duration);

    // All samples have zero duration. They have higher chance be chose by
    // mov_find_next_sample, which leads to seek again and again.
    //
    // It's AVERROR_INVALIDDATA actually, but such files exist in the wild.
    // So only mark data stream as discarded for safety.
    if (!duration && sc->stts_count &&
            st->codecpar->codec_type == AVMEDIA_TYPE_DATA) {
        av_log(c->fc, AV_LOG_WARNING,
               "All samples in data stream index:id [%d:%d] have zero "
               "duration, stream set to be discarded by default. Override "
               "using AVStream->discard or -discard for ffmpeg command.\n",
               st->index, st->id);
        st->discard = AVDISCARD_ALL;
    }
    sc->track_end = duration;
    return 0;
}
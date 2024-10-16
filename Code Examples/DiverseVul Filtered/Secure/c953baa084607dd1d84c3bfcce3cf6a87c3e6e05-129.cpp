static void mov_build_index(MOVContext *mov, AVStream *st)
{
    MOVStreamContext *sc = st->priv_data;
    FFStream *const sti = ffstream(st);
    int64_t current_offset;
    int64_t current_dts = 0;
    unsigned int stts_index = 0;
    unsigned int stsc_index = 0;
    unsigned int stss_index = 0;
    unsigned int stps_index = 0;
    unsigned int i, j;
    uint64_t stream_size = 0;
    MOVCtts *ctts_data_old = sc->ctts_data;
    unsigned int ctts_count_old = sc->ctts_count;

    int ret = build_open_gop_key_points(st);
    if (ret < 0)
        return;

    if (sc->elst_count) {
        int i, edit_start_index = 0, multiple_edits = 0;
        int64_t empty_duration = 0; // empty duration of the first edit list entry
        int64_t start_time = 0; // start time of the media

        for (i = 0; i < sc->elst_count; i++) {
            const MOVElst *e = &sc->elst_data[i];
            if (i == 0 && e->time == -1) {
                /* if empty, the first entry is the start time of the stream
                 * relative to the presentation itself */
                empty_duration = e->duration;
                edit_start_index = 1;
            } else if (i == edit_start_index && e->time >= 0) {
                start_time = e->time;
            } else {
                multiple_edits = 1;
            }
        }

        if (multiple_edits && !mov->advanced_editlist)
            av_log(mov->fc, AV_LOG_WARNING, "multiple edit list entries, "
                   "Use -advanced_editlist to correctly decode otherwise "
                   "a/v desync might occur\n");

        /* adjust first dts according to edit list */
        if ((empty_duration || start_time) && mov->time_scale > 0) {
            if (empty_duration)
                empty_duration = av_rescale(empty_duration, sc->time_scale, mov->time_scale);

            if (av_sat_sub64(start_time, empty_duration) != start_time - (uint64_t)empty_duration)
                av_log(mov->fc, AV_LOG_WARNING, "start_time - empty_duration is not representable\n");

            sc->time_offset = start_time -  (uint64_t)empty_duration;
            sc->min_corrected_pts = start_time;
            if (!mov->advanced_editlist)
                current_dts = -sc->time_offset;
        }

        if (!multiple_edits && !mov->advanced_editlist &&
            st->codecpar->codec_id == AV_CODEC_ID_AAC && start_time > 0)
            sc->start_pad = start_time;
    }

    /* only use old uncompressed audio chunk demuxing when stts specifies it */
    if (!(st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
          sc->stts_count == 1 && sc->stts_data[0].duration == 1)) {
        unsigned int current_sample = 0;
        unsigned int stts_sample = 0;
        unsigned int sample_size;
        unsigned int distance = 0;
        unsigned int rap_group_index = 0;
        unsigned int rap_group_sample = 0;
        int rap_group_present = sc->rap_group_count && sc->rap_group;
        int key_off = (sc->keyframe_count && sc->keyframes[0] > 0) || (sc->stps_count && sc->stps_data[0] > 0);

        current_dts -= sc->dts_shift;

        if (!sc->sample_count || sti->nb_index_entries)
            return;
        if (sc->sample_count >= UINT_MAX / sizeof(*sti->index_entries) - sti->nb_index_entries)
            return;
        if (av_reallocp_array(&sti->index_entries,
                              sti->nb_index_entries + sc->sample_count,
                              sizeof(*sti->index_entries)) < 0) {
            sti->nb_index_entries = 0;
            return;
        }
        sti->index_entries_allocated_size = (sti->nb_index_entries + sc->sample_count) * sizeof(*sti->index_entries);

        if (ctts_data_old) {
            // Expand ctts entries such that we have a 1-1 mapping with samples
            if (sc->sample_count >= UINT_MAX / sizeof(*sc->ctts_data))
                return;
            sc->ctts_count = 0;
            sc->ctts_allocated_size = 0;
            sc->ctts_data = av_fast_realloc(NULL, &sc->ctts_allocated_size,
                                    sc->sample_count * sizeof(*sc->ctts_data));
            if (!sc->ctts_data) {
                av_free(ctts_data_old);
                return;
            }

            memset((uint8_t*)(sc->ctts_data), 0, sc->ctts_allocated_size);

            for (i = 0; i < ctts_count_old &&
                        sc->ctts_count < sc->sample_count; i++)
                for (j = 0; j < ctts_data_old[i].count &&
                            sc->ctts_count < sc->sample_count; j++)
                    add_ctts_entry(&sc->ctts_data, &sc->ctts_count,
                                   &sc->ctts_allocated_size, 1,
                                   ctts_data_old[i].duration);
            av_free(ctts_data_old);
        }

        for (i = 0; i < sc->chunk_count; i++) {
            int64_t next_offset = i+1 < sc->chunk_count ? sc->chunk_offsets[i+1] : INT64_MAX;
            current_offset = sc->chunk_offsets[i];
            while (mov_stsc_index_valid(stsc_index, sc->stsc_count) &&
                i + 1 == sc->stsc_data[stsc_index + 1].first)
                stsc_index++;

            if (next_offset > current_offset && sc->sample_size>0 && sc->sample_size < sc->stsz_sample_size &&
                sc->stsc_data[stsc_index].count * (int64_t)sc->stsz_sample_size > next_offset - current_offset) {
                av_log(mov->fc, AV_LOG_WARNING, "STSZ sample size %d invalid (too large), ignoring\n", sc->stsz_sample_size);
                sc->stsz_sample_size = sc->sample_size;
            }
            if (sc->stsz_sample_size>0 && sc->stsz_sample_size < sc->sample_size) {
                av_log(mov->fc, AV_LOG_WARNING, "STSZ sample size %d invalid (too small), ignoring\n", sc->stsz_sample_size);
                sc->stsz_sample_size = sc->sample_size;
            }

            for (j = 0; j < sc->stsc_data[stsc_index].count; j++) {
                int keyframe = 0;
                if (current_sample >= sc->sample_count) {
                    av_log(mov->fc, AV_LOG_ERROR, "wrong sample count\n");
                    return;
                }

                if (!sc->keyframe_absent && (!sc->keyframe_count || current_sample+key_off == sc->keyframes[stss_index])) {
                    keyframe = 1;
                    if (stss_index + 1 < sc->keyframe_count)
                        stss_index++;
                } else if (sc->stps_count && current_sample+key_off == sc->stps_data[stps_index]) {
                    keyframe = 1;
                    if (stps_index + 1 < sc->stps_count)
                        stps_index++;
                }
                if (rap_group_present && rap_group_index < sc->rap_group_count) {
                    if (sc->rap_group[rap_group_index].index > 0)
                        keyframe = 1;
                    if (++rap_group_sample == sc->rap_group[rap_group_index].count) {
                        rap_group_sample = 0;
                        rap_group_index++;
                    }
                }
                if (sc->keyframe_absent
                    && !sc->stps_count
                    && !rap_group_present
                    && (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO || (i==0 && j==0)))
                     keyframe = 1;
                if (keyframe)
                    distance = 0;
                sample_size = sc->stsz_sample_size > 0 ? sc->stsz_sample_size : sc->sample_sizes[current_sample];
                if (sc->pseudo_stream_id == -1 ||
                   sc->stsc_data[stsc_index].id - 1 == sc->pseudo_stream_id) {
                    AVIndexEntry *e;
                    if (sample_size > 0x3FFFFFFF) {
                        av_log(mov->fc, AV_LOG_ERROR, "Sample size %u is too large\n", sample_size);
                        return;
                    }
                    e = &sti->index_entries[sti->nb_index_entries++];
                    e->pos = current_offset;
                    e->timestamp = current_dts;
                    e->size = sample_size;
                    e->min_distance = distance;
                    e->flags = keyframe ? AVINDEX_KEYFRAME : 0;
                    av_log(mov->fc, AV_LOG_TRACE, "AVIndex stream %d, sample %u, offset %"PRIx64", dts %"PRId64", "
                            "size %u, distance %u, keyframe %d\n", st->index, current_sample,
                            current_offset, current_dts, sample_size, distance, keyframe);
                    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && sti->nb_index_entries < 100)
                        ff_rfps_add_frame(mov->fc, st, current_dts);
                }

                current_offset += sample_size;
                stream_size += sample_size;

                current_dts += sc->stts_data[stts_index].duration;

                distance++;
                stts_sample++;
                current_sample++;
                if (stts_index + 1 < sc->stts_count && stts_sample == sc->stts_data[stts_index].count) {
                    stts_sample = 0;
                    stts_index++;
                }
            }
        }
        if (st->duration > 0)
            st->codecpar->bit_rate = stream_size*8*sc->time_scale/st->duration;
    } else {
        unsigned chunk_samples, total = 0;

        if (!sc->chunk_count)
            return;

        // compute total chunk count
        for (i = 0; i < sc->stsc_count; i++) {
            unsigned count, chunk_count;

            chunk_samples = sc->stsc_data[i].count;
            if (i != sc->stsc_count - 1 &&
                sc->samples_per_frame && chunk_samples % sc->samples_per_frame) {
                av_log(mov->fc, AV_LOG_ERROR, "error unaligned chunk\n");
                return;
            }

            if (sc->samples_per_frame >= 160) { // gsm
                count = chunk_samples / sc->samples_per_frame;
            } else if (sc->samples_per_frame > 1) {
                unsigned samples = (1024/sc->samples_per_frame)*sc->samples_per_frame;
                count = (chunk_samples+samples-1) / samples;
            } else {
                count = (chunk_samples+1023) / 1024;
            }

            if (mov_stsc_index_valid(i, sc->stsc_count))
                chunk_count = sc->stsc_data[i+1].first - sc->stsc_data[i].first;
            else
                chunk_count = sc->chunk_count - (sc->stsc_data[i].first - 1);
            total += chunk_count * count;
        }

        av_log(mov->fc, AV_LOG_TRACE, "chunk count %u\n", total);
        if (total >= UINT_MAX / sizeof(*sti->index_entries) - sti->nb_index_entries)
            return;
        if (av_reallocp_array(&sti->index_entries,
                              sti->nb_index_entries + total,
                              sizeof(*sti->index_entries)) < 0) {
            sti->nb_index_entries = 0;
            return;
        }
        sti->index_entries_allocated_size = (sti->nb_index_entries + total) * sizeof(*sti->index_entries);

        // populate index
        for (i = 0; i < sc->chunk_count; i++) {
            current_offset = sc->chunk_offsets[i];
            if (mov_stsc_index_valid(stsc_index, sc->stsc_count) &&
                i + 1 == sc->stsc_data[stsc_index + 1].first)
                stsc_index++;
            chunk_samples = sc->stsc_data[stsc_index].count;

            while (chunk_samples > 0) {
                AVIndexEntry *e;
                unsigned size, samples;

                if (sc->samples_per_frame > 1 && !sc->bytes_per_frame) {
                    avpriv_request_sample(mov->fc,
                           "Zero bytes per frame, but %d samples per frame",
                           sc->samples_per_frame);
                    return;
                }

                if (sc->samples_per_frame >= 160) { // gsm
                    samples = sc->samples_per_frame;
                    size = sc->bytes_per_frame;
                } else {
                    if (sc->samples_per_frame > 1) {
                        samples = FFMIN((1024 / sc->samples_per_frame)*
                                        sc->samples_per_frame, chunk_samples);
                        size = (samples / sc->samples_per_frame) * sc->bytes_per_frame;
                    } else {
                        samples = FFMIN(1024, chunk_samples);
                        size = samples * sc->sample_size;
                    }
                }

                if (sti->nb_index_entries >= total) {
                    av_log(mov->fc, AV_LOG_ERROR, "wrong chunk count %u\n", total);
                    return;
                }
                if (size > 0x3FFFFFFF) {
                    av_log(mov->fc, AV_LOG_ERROR, "Sample size %u is too large\n", size);
                    return;
                }
                e = &sti->index_entries[sti->nb_index_entries++];
                e->pos = current_offset;
                e->timestamp = current_dts;
                e->size = size;
                e->min_distance = 0;
                e->flags = AVINDEX_KEYFRAME;
                av_log(mov->fc, AV_LOG_TRACE, "AVIndex stream %d, chunk %u, offset %"PRIx64", dts %"PRId64", "
                       "size %u, duration %u\n", st->index, i, current_offset, current_dts,
                       size, samples);

                current_offset += size;
                current_dts += samples;
                chunk_samples -= samples;
            }
        }
    }

    if (!mov->ignore_editlist && mov->advanced_editlist) {
        // Fix index according to edit lists.
        mov_fix_index(mov, st);
    }

    // Update start time of the stream.
    if (st->start_time == AV_NOPTS_VALUE && st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && sti->nb_index_entries > 0) {
        st->start_time = sti->index_entries[0].timestamp + sc->dts_shift;
        if (sc->ctts_data) {
            st->start_time += sc->ctts_data[0].duration;
        }
    }

    mov_estimate_video_delay(mov, st);
}
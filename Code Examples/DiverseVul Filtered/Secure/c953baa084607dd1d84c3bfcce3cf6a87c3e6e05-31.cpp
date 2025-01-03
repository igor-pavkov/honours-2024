static int mov_read_trun(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVFragment *frag = &c->fragment;
    AVStream *st = NULL;
    FFStream *sti = NULL;
    MOVStreamContext *sc;
    MOVCtts *ctts_data;
    uint64_t offset;
    int64_t dts, pts = AV_NOPTS_VALUE;
    int data_offset = 0;
    unsigned entries, first_sample_flags = frag->flags;
    int flags, distance, i;
    int64_t prev_dts = AV_NOPTS_VALUE;
    int next_frag_index = -1, index_entry_pos;
    size_t requested_size;
    size_t old_ctts_allocated_size;
    AVIndexEntry *new_entries;
    MOVFragmentStreamInfo * frag_stream_info;

    if (!frag->found_tfhd) {
        av_log(c->fc, AV_LOG_ERROR, "trun track id unknown, no tfhd was found\n");
        return AVERROR_INVALIDDATA;
    }

    for (i = 0; i < c->fc->nb_streams; i++) {
        if (c->fc->streams[i]->id == frag->track_id) {
            st = c->fc->streams[i];
            sti = ffstream(st);
            break;
        }
    }
    if (!st) {
        av_log(c->fc, AV_LOG_WARNING, "could not find corresponding track id %u\n", frag->track_id);
        return 0;
    }
    sc = st->priv_data;
    if (sc->pseudo_stream_id+1 != frag->stsd_id && sc->pseudo_stream_id != -1)
        return 0;

    // Find the next frag_index index that has a valid index_entry for
    // the current track_id.
    //
    // A valid index_entry means the trun for the fragment was read
    // and it's samples are in index_entries at the given position.
    // New index entries will be inserted before the index_entry found.
    index_entry_pos = sti->nb_index_entries;
    for (i = c->frag_index.current + 1; i < c->frag_index.nb_items; i++) {
        frag_stream_info = get_frag_stream_info(&c->frag_index, i, frag->track_id);
        if (frag_stream_info && frag_stream_info->index_entry >= 0) {
            next_frag_index = i;
            index_entry_pos = frag_stream_info->index_entry;
            break;
        }
    }
    av_assert0(index_entry_pos <= sti->nb_index_entries);

    avio_r8(pb); /* version */
    flags = avio_rb24(pb);
    entries = avio_rb32(pb);
    av_log(c->fc, AV_LOG_TRACE, "flags 0x%x entries %u\n", flags, entries);

    if ((uint64_t)entries+sc->ctts_count >= UINT_MAX/sizeof(*sc->ctts_data))
        return AVERROR_INVALIDDATA;
    if (flags & MOV_TRUN_DATA_OFFSET)        data_offset        = avio_rb32(pb);
    if (flags & MOV_TRUN_FIRST_SAMPLE_FLAGS) first_sample_flags = avio_rb32(pb);

    frag_stream_info = get_current_frag_stream_info(&c->frag_index);
    if (frag_stream_info) {
        if (frag_stream_info->next_trun_dts != AV_NOPTS_VALUE) {
            dts = frag_stream_info->next_trun_dts - sc->time_offset;
        } else if (frag_stream_info->first_tfra_pts != AV_NOPTS_VALUE &&
            c->use_mfra_for == FF_MOV_FLAG_MFRA_PTS) {
            pts = frag_stream_info->first_tfra_pts;
            av_log(c->fc, AV_LOG_DEBUG, "found mfra time %"PRId64
                    ", using it for pts\n", pts);
        } else if (frag_stream_info->first_tfra_pts != AV_NOPTS_VALUE &&
            c->use_mfra_for == FF_MOV_FLAG_MFRA_DTS) {
            dts = frag_stream_info->first_tfra_pts;
            av_log(c->fc, AV_LOG_DEBUG, "found mfra time %"PRId64
                    ", using it for dts\n", pts);
        } else {
            int has_tfdt = frag_stream_info->tfdt_dts != AV_NOPTS_VALUE;
            int has_sidx = frag_stream_info->sidx_pts != AV_NOPTS_VALUE;
            int fallback_tfdt = !c->use_tfdt && !has_sidx && has_tfdt;
            int fallback_sidx =  c->use_tfdt && !has_tfdt && has_sidx;

            if (fallback_sidx) {
                av_log(c->fc, AV_LOG_DEBUG, "use_tfdt set but no tfdt found, using sidx instead\n");
            }
            if (fallback_tfdt) {
                av_log(c->fc, AV_LOG_DEBUG, "use_tfdt not set but no sidx found, using tfdt instead\n");
            }

            if (has_tfdt && c->use_tfdt || fallback_tfdt) {
                dts = frag_stream_info->tfdt_dts - sc->time_offset;
                av_log(c->fc, AV_LOG_DEBUG, "found tfdt time %"PRId64
                        ", using it for dts\n", dts);
            } else if (has_sidx && !c->use_tfdt || fallback_sidx) {
                // FIXME: sidx earliest_presentation_time is *PTS*, s.b.
                // pts = frag_stream_info->sidx_pts;
                dts = frag_stream_info->sidx_pts - sc->time_offset;
                av_log(c->fc, AV_LOG_DEBUG, "found sidx time %"PRId64
                        ", using it for dts\n", frag_stream_info->sidx_pts);
            } else {
                dts = sc->track_end - sc->time_offset;
                av_log(c->fc, AV_LOG_DEBUG, "found track end time %"PRId64
                        ", using it for dts\n", dts);
            }
        }
    } else {
        dts = sc->track_end - sc->time_offset;
        av_log(c->fc, AV_LOG_DEBUG, "found track end time %"PRId64
                ", using it for dts\n", dts);
    }
    offset   = frag->base_data_offset + data_offset;
    distance = 0;
    av_log(c->fc, AV_LOG_TRACE, "first sample flags 0x%x\n", first_sample_flags);

    // realloc space for new index entries
    if ((uint64_t)sti->nb_index_entries + entries >= UINT_MAX / sizeof(AVIndexEntry)) {
        entries = UINT_MAX / sizeof(AVIndexEntry) - sti->nb_index_entries;
        av_log(c->fc, AV_LOG_ERROR, "Failed to add index entry\n");
    }
    if (entries == 0)
        return 0;

    requested_size = (sti->nb_index_entries + entries) * sizeof(AVIndexEntry);
    new_entries = av_fast_realloc(sti->index_entries,
                                  &sti->index_entries_allocated_size,
                                  requested_size);
    if (!new_entries)
        return AVERROR(ENOMEM);
    sti->index_entries= new_entries;

    requested_size = (sti->nb_index_entries + entries) * sizeof(*sc->ctts_data);
    old_ctts_allocated_size = sc->ctts_allocated_size;
    ctts_data = av_fast_realloc(sc->ctts_data, &sc->ctts_allocated_size,
                                requested_size);
    if (!ctts_data)
        return AVERROR(ENOMEM);
    sc->ctts_data = ctts_data;

    // In case there were samples without ctts entries, ensure they get
    // zero valued entries. This ensures clips which mix boxes with and
    // without ctts entries don't pickup uninitialized data.
    memset((uint8_t*)(sc->ctts_data) + old_ctts_allocated_size, 0,
           sc->ctts_allocated_size - old_ctts_allocated_size);

    if (index_entry_pos < sti->nb_index_entries) {
        // Make hole in index_entries and ctts_data for new samples
        memmove(sti->index_entries + index_entry_pos + entries,
                sti->index_entries + index_entry_pos,
                sizeof(*sti->index_entries) *
                (sti->nb_index_entries - index_entry_pos));
        memmove(sc->ctts_data + index_entry_pos + entries,
                sc->ctts_data + index_entry_pos,
                sizeof(*sc->ctts_data) * (sc->ctts_count - index_entry_pos));
        if (index_entry_pos < sc->current_sample) {
            sc->current_sample += entries;
        }
    }

    sti->nb_index_entries += entries;
    sc->ctts_count = sti->nb_index_entries;

    // Record the index_entry position in frag_index of this fragment
    if (frag_stream_info) {
        frag_stream_info->index_entry = index_entry_pos;
        if (frag_stream_info->index_base < 0)
            frag_stream_info->index_base = index_entry_pos;
    }

    if (index_entry_pos > 0)
        prev_dts = sti->index_entries[index_entry_pos-1].timestamp;

    for (i = 0; i < entries && !pb->eof_reached; i++) {
        unsigned sample_size = frag->size;
        int sample_flags = i ? frag->flags : first_sample_flags;
        unsigned sample_duration = frag->duration;
        unsigned ctts_duration = 0;
        int keyframe = 0;
        int index_entry_flags = 0;

        if (flags & MOV_TRUN_SAMPLE_DURATION) sample_duration = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_SIZE)     sample_size     = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_FLAGS)    sample_flags    = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_CTS)      ctts_duration   = avio_rb32(pb);

        mov_update_dts_shift(sc, ctts_duration, c->fc);
        if (pts != AV_NOPTS_VALUE) {
            dts = pts - sc->dts_shift;
            if (flags & MOV_TRUN_SAMPLE_CTS) {
                dts -= ctts_duration;
            } else {
                dts -= sc->time_offset;
            }
            av_log(c->fc, AV_LOG_DEBUG,
                   "pts %"PRId64" calculated dts %"PRId64
                   " sc->dts_shift %d ctts.duration %d"
                   " sc->time_offset %"PRId64
                   " flags & MOV_TRUN_SAMPLE_CTS %d\n",
                   pts, dts,
                   sc->dts_shift, ctts_duration,
                   sc->time_offset, flags & MOV_TRUN_SAMPLE_CTS);
            pts = AV_NOPTS_VALUE;
        }

        if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            keyframe = 1;
        else
            keyframe =
                !(sample_flags & (MOV_FRAG_SAMPLE_FLAG_IS_NON_SYNC |
                                  MOV_FRAG_SAMPLE_FLAG_DEPENDS_YES));
        if (keyframe) {
            distance = 0;
            index_entry_flags |= AVINDEX_KEYFRAME;
        }
        // Fragments can overlap in time.  Discard overlapping frames after
        // decoding.
        if (prev_dts >= dts)
            index_entry_flags |= AVINDEX_DISCARD_FRAME;

        sti->index_entries[index_entry_pos].pos   = offset;
        sti->index_entries[index_entry_pos].timestamp = dts;
        sti->index_entries[index_entry_pos].size  = sample_size;
        sti->index_entries[index_entry_pos].min_distance = distance;
        sti->index_entries[index_entry_pos].flags = index_entry_flags;

        sc->ctts_data[index_entry_pos].count = 1;
        sc->ctts_data[index_entry_pos].duration = ctts_duration;
        index_entry_pos++;

        av_log(c->fc, AV_LOG_TRACE, "AVIndex stream %d, sample %d, offset %"PRIx64", dts %"PRId64", "
                "size %u, distance %d, keyframe %d\n", st->index,
                index_entry_pos, offset, dts, sample_size, distance, keyframe);
        distance++;
        if (av_sat_add64(dts, sample_duration) != dts + (uint64_t)sample_duration)
            return AVERROR_INVALIDDATA;
        if (!sample_size)
            return AVERROR_INVALIDDATA;
        dts += sample_duration;
        offset += sample_size;
        sc->data_size += sample_size;

        if (sample_duration <= INT64_MAX - sc->duration_for_fps &&
            1 <= INT_MAX - sc->nb_frames_for_fps
        ) {
            sc->duration_for_fps += sample_duration;
            sc->nb_frames_for_fps ++;
        }
    }
    if (frag_stream_info)
        frag_stream_info->next_trun_dts = dts + sc->time_offset;
    if (i < entries) {
        // EOF found before reading all entries.  Fix the hole this would
        // leave in index_entries and ctts_data
        int gap = entries - i;
        memmove(sti->index_entries + index_entry_pos,
                sti->index_entries + index_entry_pos + gap,
                sizeof(*sti->index_entries) *
                (sti->nb_index_entries - (index_entry_pos + gap)));
        memmove(sc->ctts_data + index_entry_pos,
                sc->ctts_data + index_entry_pos + gap,
                sizeof(*sc->ctts_data) *
                (sc->ctts_count - (index_entry_pos + gap)));

        sti->nb_index_entries -= gap;
        sc->ctts_count -= gap;
        if (index_entry_pos < sc->current_sample) {
            sc->current_sample -= gap;
        }
        entries = i;
    }

    // The end of this new fragment may overlap in time with the start
    // of the next fragment in index_entries. Mark the samples in the next
    // fragment that overlap with AVINDEX_DISCARD_FRAME
    prev_dts = AV_NOPTS_VALUE;
    if (index_entry_pos > 0)
        prev_dts = sti->index_entries[index_entry_pos-1].timestamp;
    for (int i = index_entry_pos; i < sti->nb_index_entries; i++) {
        if (prev_dts < sti->index_entries[i].timestamp)
            break;
        sti->index_entries[i].flags |= AVINDEX_DISCARD_FRAME;
    }

    // If a hole was created to insert the new index_entries into,
    // the index_entry recorded for all subsequent moof must
    // be incremented by the number of entries inserted.
    fix_frag_index_entries(&c->frag_index, next_frag_index,
                           frag->track_id, entries);

    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_WARNING, "reached eof, corrupted TRUN atom\n");
        return AVERROR_EOF;
    }

    frag->implicit_offset = offset;

    sc->track_end = dts + sc->time_offset;
    if (st->duration < sc->track_end)
        st->duration = sc->track_end;

    return 0;
}
static int find_prev_closest_index(AVStream *st,
                                   AVIndexEntry *e_old,
                                   int nb_old,
                                   MOVCtts* ctts_data,
                                   int64_t ctts_count,
                                   int64_t timestamp_pts,
                                   int flag,
                                   int64_t* index,
                                   int64_t* ctts_index,
                                   int64_t* ctts_sample)
{
    MOVStreamContext *msc = st->priv_data;
    FFStream *const sti = ffstream(st);
    AVIndexEntry *e_keep = sti->index_entries;
    int nb_keep = sti->nb_index_entries;
    int64_t i = 0;
    int64_t index_ctts_count;

    av_assert0(index);

    // If dts_shift > 0, then all the index timestamps will have to be offset by
    // at least dts_shift amount to obtain PTS.
    // Hence we decrement the searched timestamp_pts by dts_shift to find the closest index element.
    if (msc->dts_shift > 0) {
        timestamp_pts -= msc->dts_shift;
    }

    sti->index_entries    = e_old;
    sti->nb_index_entries = nb_old;
    *index = av_index_search_timestamp(st, timestamp_pts, flag | AVSEEK_FLAG_BACKWARD);

    // Keep going backwards in the index entries until the timestamp is the same.
    if (*index >= 0) {
        for (i = *index; i > 0 && e_old[i].timestamp == e_old[i - 1].timestamp;
             i--) {
            if ((flag & AVSEEK_FLAG_ANY) ||
                (e_old[i - 1].flags & AVINDEX_KEYFRAME)) {
                *index = i - 1;
            }
        }
    }

    // If we have CTTS then refine the search, by searching backwards over PTS
    // computed by adding corresponding CTTS durations to index timestamps.
    if (ctts_data && *index >= 0) {
        av_assert0(ctts_index);
        av_assert0(ctts_sample);
        // Find out the ctts_index for the found frame.
        *ctts_index = 0;
        *ctts_sample = 0;
        for (index_ctts_count = 0; index_ctts_count < *index; index_ctts_count++) {
            if (*ctts_index < ctts_count) {
                (*ctts_sample)++;
                if (ctts_data[*ctts_index].count == *ctts_sample) {
                    (*ctts_index)++;
                    *ctts_sample = 0;
                }
            }
        }

        while (*index >= 0 && (*ctts_index) >= 0 && (*ctts_index) < ctts_count) {
            // Find a "key frame" with PTS <= timestamp_pts (So that we can decode B-frames correctly).
            // No need to add dts_shift to the timestamp here becase timestamp_pts has already been
            // compensated by dts_shift above.
            if ((e_old[*index].timestamp + ctts_data[*ctts_index].duration) <= timestamp_pts &&
                (e_old[*index].flags & AVINDEX_KEYFRAME)) {
                break;
            }

            (*index)--;
            if (*ctts_sample == 0) {
                (*ctts_index)--;
                if (*ctts_index >= 0)
                  *ctts_sample = ctts_data[*ctts_index].count - 1;
            } else {
                (*ctts_sample)--;
            }
        }
    }

    /* restore AVStream state*/
    sti->index_entries    = e_keep;
    sti->nb_index_entries = nb_keep;
    return *index >= 0 ? 0 : -1;
}
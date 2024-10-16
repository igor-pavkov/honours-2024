static int mov_add_tfra_entries(AVIOContext *pb, MOVMuxContext *mov, int tracks,
                                int size)
{
    int i;
    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        MOVFragmentInfo *info;
        if ((tracks >= 0 && i != tracks) || !track->entry)
            continue;
        track->nb_frag_info++;
        if (track->nb_frag_info >= track->frag_info_capacity) {
            unsigned new_capacity = track->nb_frag_info + MOV_FRAG_INFO_ALLOC_INCREMENT;
            if (av_reallocp_array(&track->frag_info,
                                  new_capacity,
                                  sizeof(*track->frag_info)))
                return AVERROR(ENOMEM);
            track->frag_info_capacity = new_capacity;
        }
        info = &track->frag_info[track->nb_frag_info - 1];
        info->offset   = avio_tell(pb);
        info->size     = size;
        // Try to recreate the original pts for the first packet
        // from the fields we have stored
        info->time     = track->start_dts + track->frag_start +
                         track->cluster[0].cts;
        info->duration = track->end_pts -
                         (track->cluster[0].dts + track->cluster[0].cts);
        // If the pts is less than zero, we will have trimmed
        // away parts of the media track using an edit list,
        // and the corresponding start presentation time is zero.
        if (info->time < 0) {
            info->duration += info->time;
            info->time = 0;
        }
        info->tfrf_offset = 0;
        mov_write_tfrf_tags(pb, mov, track);
    }
    return 0;
}
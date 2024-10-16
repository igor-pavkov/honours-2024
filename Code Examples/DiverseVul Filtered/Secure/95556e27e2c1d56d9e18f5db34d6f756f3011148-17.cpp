static void mov_prune_frag_info(MOVMuxContext *mov, int tracks, int max)
{
    int i;
    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *track = &mov->tracks[i];
        if ((tracks >= 0 && i != tracks) || !track->entry)
            continue;
        if (track->nb_frag_info > max) {
            memmove(track->frag_info, track->frag_info + (track->nb_frag_info - max), max * sizeof(*track->frag_info));
            track->nb_frag_info = max;
        }
    }
}
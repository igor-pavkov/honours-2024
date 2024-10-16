static int mov_write_tfrf_tags(AVIOContext *pb, MOVMuxContext *mov,
                               MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    int i;
    for (i = 0; i < mov->ism_lookahead; i++) {
        /* Update the tfrf tag for the last ism_lookahead fragments,
         * nb_frag_info - 1 is the next fragment to be written. */
        mov_write_tfrf_tag(pb, mov, track, track->nb_frag_info - 2 - i);
    }
    avio_seek(pb, pos, SEEK_SET);
    return 0;
}
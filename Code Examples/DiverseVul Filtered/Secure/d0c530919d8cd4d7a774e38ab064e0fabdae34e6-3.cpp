static void demux_seek_mf(demuxer_t *demuxer, double seek_pts, int flags)
{
    mf_t *mf = demuxer->priv;
    double newpos = seek_pts * mf->sh->codec->fps;
    if (flags & SEEK_FACTOR)
        newpos = seek_pts * (mf->nr_of_files - 1);
    if (flags & SEEK_FORWARD) {
        newpos = ceil(newpos);
    } else {
        newpos = MPMIN(floor(newpos), mf->nr_of_files - 1);
    }
    mf->curr_frame = MPCLAMP((int)newpos, 0, mf->nr_of_files);
}
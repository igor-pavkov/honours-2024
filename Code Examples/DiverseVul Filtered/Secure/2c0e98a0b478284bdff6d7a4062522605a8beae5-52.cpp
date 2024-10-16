static int mov_write_extradata_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_write(pb, track->par->extradata, track->par->extradata_size);
    return track->par->extradata_size;
}
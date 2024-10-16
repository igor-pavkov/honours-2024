static int mov_write_subtitle_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);    /* size */
    avio_wl32(pb, track->tag); // store it byteswapped
    avio_wb32(pb, 0);    /* Reserved */
    avio_wb16(pb, 0);    /* Reserved */
    avio_wb16(pb, 1);    /* Data-reference index */

    if (track->par->codec_id == AV_CODEC_ID_DVD_SUBTITLE)
        mov_write_esds_tag(pb, track);
    else if (track->par->extradata_size)
        avio_write(pb, track->par->extradata, track->par->extradata_size);

    return update_size(pb, pos);
}
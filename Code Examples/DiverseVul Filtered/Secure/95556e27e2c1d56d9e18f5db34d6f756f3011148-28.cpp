static int mov_write_dops_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "dOps");
    avio_w8(pb, 0); /* Version */
    if (track->par->extradata_size < 19) {
        av_log(pb, AV_LOG_ERROR, "invalid extradata size\n");
        return AVERROR_INVALIDDATA;
    }
    /* extradata contains an Ogg OpusHead, other than byte-ordering and
       OpusHead's preceeding magic/version, OpusSpecificBox is currently
       identical. */
    avio_w8(pb, AV_RB8(track->par->extradata + 9)); /* OuputChannelCount */
    avio_wb16(pb, AV_RL16(track->par->extradata + 10)); /* PreSkip */
    avio_wb32(pb, AV_RL32(track->par->extradata + 12)); /* InputSampleRate */
    avio_wb16(pb, AV_RL16(track->par->extradata + 16)); /* OutputGain */
    /* Write the rest of the header out without byte-swapping. */
    avio_write(pb, track->par->extradata + 18, track->par->extradata_size - 18);

    return update_size(pb, pos);
}
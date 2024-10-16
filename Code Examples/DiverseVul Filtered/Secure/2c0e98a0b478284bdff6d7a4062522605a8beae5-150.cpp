static int mov_write_chan_tag(AVFormatContext *s, AVIOContext *pb, MOVTrack *track)
{
    uint32_t layout_tag, bitmap;
    int64_t pos = avio_tell(pb);

    layout_tag = ff_mov_get_channel_layout_tag(track->par->codec_id,
                                               track->par->channel_layout,
                                               &bitmap);
    if (!layout_tag) {
        av_log(s, AV_LOG_WARNING, "not writing 'chan' tag due to "
               "lack of channel information\n");
        return 0;
    }

    if (track->multichannel_as_mono)
        return 0;

    avio_wb32(pb, 0);           // Size
    ffio_wfourcc(pb, "chan");   // Type
    avio_w8(pb, 0);             // Version
    avio_wb24(pb, 0);           // Flags
    avio_wb32(pb, layout_tag);  // mChannelLayoutTag
    avio_wb32(pb, bitmap);      // mChannelBitmap
    avio_wb32(pb, 0);           // mNumberChannelDescriptions

    return update_size(pb, pos);
}
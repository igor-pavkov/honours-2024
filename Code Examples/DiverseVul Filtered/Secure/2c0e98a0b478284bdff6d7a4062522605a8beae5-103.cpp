static int mov_write_esds_tag(AVIOContext *pb, MOVTrack *track) // Basic
{
    AVCPBProperties *props;
    int64_t pos = avio_tell(pb);
    int decoder_specific_info_len = track->vos_len ? 5 + track->vos_len : 0;
    unsigned avg_bitrate;

    avio_wb32(pb, 0); // size
    ffio_wfourcc(pb, "esds");
    avio_wb32(pb, 0); // Version

    // ES descriptor
    put_descr(pb, 0x03, 3 + 5+13 + decoder_specific_info_len + 5+1);
    avio_wb16(pb, track->track_id);
    avio_w8(pb, 0x00); // flags (= no flags)

    // DecoderConfig descriptor
    put_descr(pb, 0x04, 13 + decoder_specific_info_len);

    // Object type indication
    if ((track->par->codec_id == AV_CODEC_ID_MP2 ||
         track->par->codec_id == AV_CODEC_ID_MP3) &&
        track->par->sample_rate > 24000)
        avio_w8(pb, 0x6B); // 11172-3
    else
        avio_w8(pb, ff_codec_get_tag(ff_mp4_obj_type, track->par->codec_id));

    // the following fields is made of 6 bits to identify the streamtype (4 for video, 5 for audio)
    // plus 1 bit to indicate upstream and 1 bit set to 1 (reserved)
    if (track->par->codec_id == AV_CODEC_ID_DVD_SUBTITLE)
        avio_w8(pb, (0x38 << 2) | 1); // flags (= NeroSubpicStream)
    else if (track->par->codec_type == AVMEDIA_TYPE_AUDIO)
        avio_w8(pb, 0x15); // flags (= Audiostream)
    else
        avio_w8(pb, 0x11); // flags (= Visualstream)

    props = (AVCPBProperties*)av_stream_get_side_data(track->st, AV_PKT_DATA_CPB_PROPERTIES,
                                                      NULL);

    avio_wb24(pb, props ? props->buffer_size / 8 : 0); // Buffersize DB

    avg_bitrate = compute_avg_bitrate(track);
    avio_wb32(pb, props ? FFMAX3(props->max_bitrate, props->avg_bitrate, avg_bitrate) : FFMAX(track->par->bit_rate, avg_bitrate)); // maxbitrate (FIXME should be max rate in any 1 sec window)
    avio_wb32(pb, avg_bitrate);

    if (track->vos_len) {
        // DecoderSpecific info descriptor
        put_descr(pb, 0x05, track->vos_len);
        avio_write(pb, track->vos_data, track->vos_len);
    }

    // SL descriptor
    put_descr(pb, 0x06, 1);
    avio_w8(pb, 0x02);
    return update_size(pb, pos);
}
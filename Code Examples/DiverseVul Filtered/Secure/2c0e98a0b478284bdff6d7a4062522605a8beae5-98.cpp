static int mov_create_timecode_track(AVFormatContext *s, int index, int src_index, AVTimecode tc)
{
    int ret;
    MOVMuxContext *mov  = s->priv_data;
    MOVTrack *track     = &mov->tracks[index];
    AVStream *src_st    = s->streams[src_index];
    AVPacket pkt    = {.stream_index = index, .flags = AV_PKT_FLAG_KEY, .size = 4};
    AVRational rate = find_fps(s, src_st);

    /* tmcd track based on video stream */
    track->mode      = mov->mode;
    track->tag       = MKTAG('t','m','c','d');
    track->src_track = src_index;
    track->timescale = mov->tracks[src_index].timescale;
    if (tc.flags & AV_TIMECODE_FLAG_DROPFRAME)
        track->timecode_flags |= MOV_TIMECODE_FLAG_DROPFRAME;

    /* set st to src_st for metadata access*/
    track->st = src_st;

    /* encode context: tmcd data stream */
    track->par = avcodec_parameters_alloc();
    if (!track->par)
        return AVERROR(ENOMEM);
    track->par->codec_type = AVMEDIA_TYPE_DATA;
    track->par->codec_tag  = track->tag;
    track->st->avg_frame_rate = av_inv_q(rate);

    /* the tmcd track just contains one packet with the frame number */
    pkt.data = av_malloc(pkt.size);
    if (!pkt.data)
        return AVERROR(ENOMEM);
    AV_WB32(pkt.data, tc.start);
    ret = ff_mov_write_packet(s, &pkt);
    av_free(pkt.data);
    return ret;
}
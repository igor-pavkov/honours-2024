static int mov_write_tmcd_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
#if 1
    int frame_duration;
    int nb_frames;
    AVDictionaryEntry *t = NULL;

    if (!track->st->avg_frame_rate.num || !track->st->avg_frame_rate.den) {
#if FF_API_LAVF_AVCTX
    FF_DISABLE_DEPRECATION_WARNINGS
        frame_duration = av_rescale(track->timescale, track->st->codec->time_base.num, track->st->codec->time_base.den);
        nb_frames      = ROUNDED_DIV(track->st->codec->time_base.den, track->st->codec->time_base.num);
    FF_ENABLE_DEPRECATION_WARNINGS
#else
        av_log(NULL, AV_LOG_ERROR, "avg_frame_rate not set for tmcd track.\n");
        return AVERROR(EINVAL);
#endif
    } else {
        frame_duration = av_rescale(track->timescale, track->st->avg_frame_rate.num, track->st->avg_frame_rate.den);
        nb_frames      = ROUNDED_DIV(track->st->avg_frame_rate.den, track->st->avg_frame_rate.num);
    }

    if (nb_frames > 255) {
        av_log(NULL, AV_LOG_ERROR, "fps %d is too large\n", nb_frames);
        return AVERROR(EINVAL);
    }

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "tmcd");               /* Data format */
    avio_wb32(pb, 0);                       /* Reserved */
    avio_wb32(pb, 1);                       /* Data reference index */
    avio_wb32(pb, 0);                       /* Flags */
    avio_wb32(pb, track->timecode_flags);   /* Flags (timecode) */
    avio_wb32(pb, track->timescale);        /* Timescale */
    avio_wb32(pb, frame_duration);          /* Frame duration */
    avio_w8(pb, nb_frames);                 /* Number of frames */
    avio_w8(pb, 0);                         /* Reserved */

    t = av_dict_get(track->st->metadata, "reel_name", NULL, 0);
    if (t && utf8len(t->value) && track->mode != MODE_MP4)
        mov_write_source_reference_tag(pb, track, t->value);
    else
        avio_wb16(pb, 0); /* zero size */
#else

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "tmcd");               /* Data format */
    avio_wb32(pb, 0);                       /* Reserved */
    avio_wb32(pb, 1);                       /* Data reference index */
    if (track->par->extradata_size)
        avio_write(pb, track->par->extradata, track->par->extradata_size);
#endif
    return update_size(pb, pos);
}
static int mov_parse_stsd_data(MOVContext *c, AVIOContext *pb,
                                AVStream *st, MOVStreamContext *sc,
                                int64_t size)
{
    int ret;

    if (st->codecpar->codec_tag == MKTAG('t','m','c','d')) {
        if ((int)size != size)
            return AVERROR(ENOMEM);

        ret = ff_get_extradata(c->fc, st->codecpar, pb, size);
        if (ret < 0)
            return ret;
        if (size > 16) {
            MOVStreamContext *tmcd_ctx = st->priv_data;
            int val;
            val = AV_RB32(st->codecpar->extradata + 4);
            tmcd_ctx->tmcd_flags = val;
            st->avg_frame_rate.num = AV_RB32(st->codecpar->extradata + 8); /* timescale */
            st->avg_frame_rate.den = AV_RB32(st->codecpar->extradata + 12); /* frameDuration */
            tmcd_ctx->tmcd_nb_frames = st->codecpar->extradata[16]; /* number of frames */
            if (size > 30) {
                uint32_t len = AV_RB32(st->codecpar->extradata + 18); /* name atom length */
                uint32_t format = AV_RB32(st->codecpar->extradata + 22);
                if (format == AV_RB32("name") && (int64_t)size >= (int64_t)len + 18) {
                    uint16_t str_size = AV_RB16(st->codecpar->extradata + 26); /* string length */
                    if (str_size > 0 && size >= (int)str_size + 30 &&
                        st->codecpar->extradata[30] /* Don't add empty string */) {
                        char *reel_name = av_malloc(str_size + 1);
                        if (!reel_name)
                            return AVERROR(ENOMEM);
                        memcpy(reel_name, st->codecpar->extradata + 30, str_size);
                        reel_name[str_size] = 0; /* Add null terminator */
                        av_dict_set(&st->metadata, "reel_name", reel_name,
                                    AV_DICT_DONT_STRDUP_VAL);
                    }
                }
            }
        }
    } else {
        /* other codec type, just skip (rtp, mp4s ...) */
        avio_skip(pb, size);
    }
    return 0;
}
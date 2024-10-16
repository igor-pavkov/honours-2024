static int mov_finalize_stsd_codec(MOVContext *c, AVIOContext *pb,
                                   AVStream *st, MOVStreamContext *sc)
{
    FFStream *const sti = ffstream(st);

    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
        !st->codecpar->sample_rate && sc->time_scale > 1)
        st->codecpar->sample_rate = sc->time_scale;

    /* special codec parameters handling */
    switch (st->codecpar->codec_id) {
#if CONFIG_DV_DEMUXER
    case AV_CODEC_ID_DVAUDIO:
        c->dv_fctx = avformat_alloc_context();
        if (!c->dv_fctx) {
            av_log(c->fc, AV_LOG_ERROR, "dv demux context alloc error\n");
            return AVERROR(ENOMEM);
        }
        c->dv_demux = avpriv_dv_init_demux(c->dv_fctx);
        if (!c->dv_demux) {
            av_log(c->fc, AV_LOG_ERROR, "dv demux context init error\n");
            return AVERROR(ENOMEM);
        }
        sc->dv_audio_container = 1;
        st->codecpar->codec_id    = AV_CODEC_ID_PCM_S16LE;
        break;
#endif
    /* no ifdef since parameters are always those */
    case AV_CODEC_ID_QCELP:
        av_channel_layout_uninit(&st->codecpar->ch_layout);
        st->codecpar->ch_layout = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
        // force sample rate for qcelp when not stored in mov
        if (st->codecpar->codec_tag != MKTAG('Q','c','l','p'))
            st->codecpar->sample_rate = 8000;
        // FIXME: Why is the following needed for some files?
        sc->samples_per_frame = 160;
        if (!sc->bytes_per_frame)
            sc->bytes_per_frame = 35;
        break;
    case AV_CODEC_ID_AMR_NB:
        av_channel_layout_uninit(&st->codecpar->ch_layout);
        st->codecpar->ch_layout = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
        /* force sample rate for amr, stsd in 3gp does not store sample rate */
        st->codecpar->sample_rate = 8000;
        break;
    case AV_CODEC_ID_AMR_WB:
        av_channel_layout_uninit(&st->codecpar->ch_layout);
        st->codecpar->ch_layout = (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;
        st->codecpar->sample_rate = 16000;
        break;
    case AV_CODEC_ID_MP2:
    case AV_CODEC_ID_MP3:
        /* force type after stsd for m1a hdlr */
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        break;
    case AV_CODEC_ID_GSM:
    case AV_CODEC_ID_ADPCM_MS:
    case AV_CODEC_ID_ADPCM_IMA_WAV:
    case AV_CODEC_ID_ILBC:
    case AV_CODEC_ID_MACE3:
    case AV_CODEC_ID_MACE6:
    case AV_CODEC_ID_QDM2:
        st->codecpar->block_align = sc->bytes_per_frame;
        break;
    case AV_CODEC_ID_ALAC:
        if (st->codecpar->extradata_size == 36) {
            int channel_count = AV_RB8(st->codecpar->extradata + 21);
            if (st->codecpar->ch_layout.nb_channels != channel_count) {
                av_channel_layout_uninit(&st->codecpar->ch_layout);
                st->codecpar->ch_layout.order = AV_CHANNEL_ORDER_UNSPEC;
                st->codecpar->ch_layout.nb_channels = channel_count;
            }
            st->codecpar->sample_rate = AV_RB32(st->codecpar->extradata + 32);
        }
        break;
    case AV_CODEC_ID_AC3:
    case AV_CODEC_ID_EAC3:
    case AV_CODEC_ID_MPEG1VIDEO:
    case AV_CODEC_ID_VC1:
    case AV_CODEC_ID_VP8:
    case AV_CODEC_ID_VP9:
        sti->need_parsing = AVSTREAM_PARSE_FULL;
        break;
    case AV_CODEC_ID_AV1:
        /* field_order detection of H264 requires parsing */
    case AV_CODEC_ID_H264:
        sti->need_parsing = AVSTREAM_PARSE_HEADERS;
        break;
    default:
        break;
    }
    return 0;
}
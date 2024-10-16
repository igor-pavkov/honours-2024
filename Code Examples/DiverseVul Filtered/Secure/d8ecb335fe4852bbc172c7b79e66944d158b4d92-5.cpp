static int flv_write_header(AVFormatContext *s)
{
    int i;
    AVIOContext *pb = s->pb;
    FLVContext *flv = s->priv_data;
    int64_t data_size;

    for (i = 0; i < s->nb_streams; i++) {
        AVCodecContext *enc = s->streams[i]->codec;
        FLVStreamContext *sc;
        switch (enc->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            if (s->streams[i]->avg_frame_rate.den &&
                s->streams[i]->avg_frame_rate.num) {
                flv->framerate = av_q2d(s->streams[i]->avg_frame_rate);
            }
            if (flv->video_enc) {
                av_log(s, AV_LOG_ERROR,
                       "at most one video stream is supported in flv\n");
                return AVERROR(EINVAL);
            }
            flv->video_enc = enc;
            if (!ff_codec_get_tag(flv_video_codec_ids, enc->codec_id))
                return unsupported_codec(s, "Video", enc->codec_id);

            if (enc->codec_id == AV_CODEC_ID_MPEG4 ||
                enc->codec_id == AV_CODEC_ID_H263) {
                int error = s->strict_std_compliance > FF_COMPLIANCE_UNOFFICIAL;
                av_log(s, error ? AV_LOG_ERROR : AV_LOG_WARNING,
                       "Codec %s is not supported in the official FLV specification,\n", avcodec_get_name(enc->codec_id));

                if (error) {
                    av_log(s, AV_LOG_ERROR,
                           "use vstrict=-1 / -strict -1 to use it anyway.\n");
                    return AVERROR(EINVAL);
                }
            } else if (enc->codec_id == AV_CODEC_ID_VP6) {
                av_log(s, AV_LOG_WARNING,
                       "Muxing VP6 in flv will produce flipped video on playback.\n");
            }
            break;
        case AVMEDIA_TYPE_AUDIO:
            if (flv->audio_enc) {
                av_log(s, AV_LOG_ERROR,
                       "at most one audio stream is supported in flv\n");
                return AVERROR(EINVAL);
            }
            flv->audio_enc = enc;
            if (get_audio_flags(s, enc) < 0)
                return unsupported_codec(s, "Audio", enc->codec_id);
            if (enc->codec_id == AV_CODEC_ID_PCM_S16BE)
                av_log(s, AV_LOG_WARNING,
                       "16-bit big-endian audio in flv is valid but most likely unplayable (hardware dependent); use s16le\n");
            break;
        case AVMEDIA_TYPE_DATA:
            if (enc->codec_id != AV_CODEC_ID_TEXT && enc->codec_id != AV_CODEC_ID_NONE)
                return unsupported_codec(s, "Data", enc->codec_id);
            flv->data_enc = enc;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            if (enc->codec_id != AV_CODEC_ID_TEXT) {
                av_log(s, AV_LOG_ERROR, "Subtitle codec '%s' for stream %d is not compatible with FLV\n",
                       avcodec_get_name(enc->codec_id), i);
                return AVERROR_INVALIDDATA;
            }
            flv->data_enc = enc;
            break;
        default:
            av_log(s, AV_LOG_ERROR, "Codec type '%s' for stream %d is not compatible with FLV\n",
                   av_get_media_type_string(enc->codec_type), i);
            return AVERROR(EINVAL);
        }
        avpriv_set_pts_info(s->streams[i], 32, 1, 1000); /* 32 bit pts in ms */

        sc = av_mallocz(sizeof(FLVStreamContext));
        if (!sc)
            return AVERROR(ENOMEM);
        s->streams[i]->priv_data = sc;
        sc->last_ts = -1;
    }

    flv->delay = AV_NOPTS_VALUE;

    avio_write(pb, "FLV", 3);
    avio_w8(pb, 1);
    avio_w8(pb, FLV_HEADER_FLAG_HASAUDIO * !!flv->audio_enc +
                FLV_HEADER_FLAG_HASVIDEO * !!flv->video_enc);
    avio_wb32(pb, 9);
    avio_wb32(pb, 0);

    for (i = 0; i < s->nb_streams; i++)
        if (s->streams[i]->codec->codec_tag == 5) {
            avio_w8(pb, 8);     // message type
            avio_wb24(pb, 0);   // include flags
            avio_wb24(pb, 0);   // time stamp
            avio_wb32(pb, 0);   // reserved
            avio_wb32(pb, 11);  // size
            flv->reserved = 5;
        }

    write_metadata(s, 0);

    for (i = 0; i < s->nb_streams; i++) {
        AVCodecContext *enc = s->streams[i]->codec;
        if (enc->codec_id == AV_CODEC_ID_AAC || enc->codec_id == AV_CODEC_ID_H264 || enc->codec_id == AV_CODEC_ID_MPEG4) {
            int64_t pos;
            avio_w8(pb, enc->codec_type == AVMEDIA_TYPE_VIDEO ?
                    FLV_TAG_TYPE_VIDEO : FLV_TAG_TYPE_AUDIO);
            avio_wb24(pb, 0); // size patched later
            avio_wb24(pb, 0); // ts
            avio_w8(pb, 0);   // ts ext
            avio_wb24(pb, 0); // streamid
            pos = avio_tell(pb);
            if (enc->codec_id == AV_CODEC_ID_AAC) {
                avio_w8(pb, get_audio_flags(s, enc));
                avio_w8(pb, 0); // AAC sequence header
                avio_write(pb, enc->extradata, enc->extradata_size);
            } else {
                avio_w8(pb, enc->codec_tag | FLV_FRAME_KEY); // flags
                avio_w8(pb, 0); // AVC sequence header
                avio_wb24(pb, 0); // composition time
                ff_isom_write_avcc(pb, enc->extradata, enc->extradata_size);
            }
            data_size = avio_tell(pb) - pos;
            avio_seek(pb, -data_size - 10, SEEK_CUR);
            avio_wb24(pb, data_size);
            avio_skip(pb, data_size + 10 - 3);
            avio_wb32(pb, data_size + 11); // previous tag size
        }
    }

    return 0;
}
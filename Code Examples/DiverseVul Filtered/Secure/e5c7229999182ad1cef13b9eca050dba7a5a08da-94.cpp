void avcodec_string(char *buf, int buf_size, AVCodecContext *enc, int encode)
{
    const char *codec_type;
    const char *codec_name;
    const char *profile = NULL;
    const AVCodec *p;
    int bitrate;
    AVRational display_aspect_ratio;

    if (!buf || buf_size <= 0)
        return;
    codec_type = av_get_media_type_string(enc->codec_type);
    codec_name = avcodec_get_name(enc->codec_id);
    if (enc->profile != FF_PROFILE_UNKNOWN) {
        if (enc->codec)
            p = enc->codec;
        else
            p = encode ? avcodec_find_encoder(enc->codec_id) :
                        avcodec_find_decoder(enc->codec_id);
        if (p)
            profile = av_get_profile_name(p, enc->profile);
    }

    snprintf(buf, buf_size, "%s: %s", codec_type ? codec_type : "unknown",
             codec_name);
    buf[0] ^= 'a' ^ 'A'; /* first letter in uppercase */

    if (enc->codec && strcmp(enc->codec->name, codec_name))
        snprintf(buf + strlen(buf), buf_size - strlen(buf), " (%s)", enc->codec->name);

    if (profile)
        snprintf(buf + strlen(buf), buf_size - strlen(buf), " (%s)", profile);
    if (enc->codec_tag) {
        char tag_buf[32];
        av_get_codec_tag_string(tag_buf, sizeof(tag_buf), enc->codec_tag);
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 " (%s / 0x%04X)", tag_buf, enc->codec_tag);
    }

    switch (enc->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        if (enc->pix_fmt != AV_PIX_FMT_NONE) {
            char detail[256] = "(";
            const char *colorspace_name;
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %s",
                     av_get_pix_fmt_name(enc->pix_fmt));
            if (enc->bits_per_raw_sample &&
                enc->bits_per_raw_sample <= av_pix_fmt_desc_get(enc->pix_fmt)->comp[0].depth_minus1)
                av_strlcatf(detail, sizeof(detail), "%d bpc, ", enc->bits_per_raw_sample);
            if (enc->color_range != AVCOL_RANGE_UNSPECIFIED)
                av_strlcatf(detail, sizeof(detail),
                            enc->color_range == AVCOL_RANGE_MPEG ? "tv, ": "pc, ");

            colorspace_name = av_get_colorspace_name(enc->colorspace);
            if (colorspace_name)
                av_strlcatf(detail, sizeof(detail), "%s, ", colorspace_name);

            if (strlen(detail) > 1) {
                detail[strlen(detail) - 2] = 0;
                av_strlcatf(buf, buf_size, "%s)", detail);
            }
        }
        if (enc->width) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %dx%d",
                     enc->width, enc->height);
            if (enc->sample_aspect_ratio.num) {
                av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
                          enc->width * enc->sample_aspect_ratio.num,
                          enc->height * enc->sample_aspect_ratio.den,
                          1024 * 1024);
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         " [SAR %d:%d DAR %d:%d]",
                         enc->sample_aspect_ratio.num, enc->sample_aspect_ratio.den,
                         display_aspect_ratio.num, display_aspect_ratio.den);
            }
            if (av_log_get_level() >= AV_LOG_DEBUG) {
                int g = av_gcd(enc->time_base.num, enc->time_base.den);
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         ", %d/%d",
                         enc->time_base.num / g, enc->time_base.den / g);
            }
        }
        if (encode) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", q=%d-%d", enc->qmin, enc->qmax);
        }
        break;
    case AVMEDIA_TYPE_AUDIO:
        if (enc->sample_rate) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %d Hz", enc->sample_rate);
        }
        av_strlcat(buf, ", ", buf_size);
        av_get_channel_layout_string(buf + strlen(buf), buf_size - strlen(buf), enc->channels, enc->channel_layout);
        if (enc->sample_fmt != AV_SAMPLE_FMT_NONE) {
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %s", av_get_sample_fmt_name(enc->sample_fmt));
        }
        break;
    case AVMEDIA_TYPE_DATA:
        if (av_log_get_level() >= AV_LOG_DEBUG) {
            int g = av_gcd(enc->time_base.num, enc->time_base.den);
            if (g)
                snprintf(buf + strlen(buf), buf_size - strlen(buf),
                         ", %d/%d",
                         enc->time_base.num / g, enc->time_base.den / g);
        }
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        if (enc->width)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %dx%d", enc->width, enc->height);
        break;
    default:
        return;
    }
    if (encode) {
        if (enc->flags & CODEC_FLAG_PASS1)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", pass 1");
        if (enc->flags & CODEC_FLAG_PASS2)
            snprintf(buf + strlen(buf), buf_size - strlen(buf),
                     ", pass 2");
    }
    bitrate = get_bit_rate(enc);
    if (bitrate != 0) {
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 ", %d kb/s", bitrate / 1000);
    } else if (enc->rc_max_rate > 0) {
        snprintf(buf + strlen(buf), buf_size - strlen(buf),
                 ", max. %d kb/s", enc->rc_max_rate / 1000);
    }
}
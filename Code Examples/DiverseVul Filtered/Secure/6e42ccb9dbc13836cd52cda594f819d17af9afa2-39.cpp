static int decode_eld_specific_config(AACContext *ac, AVCodecContext *avctx,
                                     GetBitContext *gb,
                                     MPEG4AudioConfig *m4ac,
                                     int channel_config)
{
    int ret, ep_config, res_flags;
    uint8_t layout_map[MAX_ELEM_ID*4][3];
    int tags = 0;
    const int ELDEXT_TERM = 0;

    m4ac->ps  = 0;
    m4ac->sbr = 0;

    if (get_bits1(gb)) { // frameLengthFlag
        avpriv_request_sample(avctx, "960/120 MDCT window");
        return AVERROR_PATCHWELCOME;
    }

    res_flags = get_bits(gb, 3);
    if (res_flags) {
        avpriv_report_missing_feature(avctx,
                                      "AAC data resilience (flags %x)",
                                      res_flags);
        return AVERROR_PATCHWELCOME;
    }

    if (get_bits1(gb)) { // ldSbrPresentFlag
        avpriv_report_missing_feature(avctx,
                                      "Low Delay SBR");
        return AVERROR_PATCHWELCOME;
    }

    while (get_bits(gb, 4) != ELDEXT_TERM) {
        int len = get_bits(gb, 4);
        if (len == 15)
            len += get_bits(gb, 8);
        if (len == 15 + 255)
            len += get_bits(gb, 16);
        if (get_bits_left(gb) < len * 8 + 4) {
            av_log(ac->avctx, AV_LOG_ERROR, overread_err);
            return AVERROR_INVALIDDATA;
        }
        skip_bits_long(gb, 8 * len);
    }

    if ((ret = set_default_channel_config(avctx, layout_map,
                                          &tags, channel_config)))
        return ret;

    if (ac && (ret = output_configure(ac, layout_map, tags, OC_GLOBAL_HDR, 0)))
        return ret;

    ep_config = get_bits(gb, 2);
    if (ep_config) {
        avpriv_report_missing_feature(avctx,
                                      "epConfig %d", ep_config);
        return AVERROR_PATCHWELCOME;
    }
    return 0;
}
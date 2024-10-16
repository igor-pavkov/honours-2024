static int decode_ga_specific_config(AACContext *ac, AVCodecContext *avctx,
                                     GetBitContext *gb,
                                     MPEG4AudioConfig *m4ac,
                                     int channel_config)
{
    int extension_flag, ret, ep_config, res_flags;
    uint8_t layout_map[MAX_ELEM_ID*4][3];
    int tags = 0;

    if (get_bits1(gb)) { // frameLengthFlag
        avpriv_request_sample(avctx, "960/120 MDCT window");
        return AVERROR_PATCHWELCOME;
    }

    if (get_bits1(gb))       // dependsOnCoreCoder
        skip_bits(gb, 14);   // coreCoderDelay
    extension_flag = get_bits1(gb);

    if (m4ac->object_type == AOT_AAC_SCALABLE ||
        m4ac->object_type == AOT_ER_AAC_SCALABLE)
        skip_bits(gb, 3);     // layerNr

    if (channel_config == 0) {
        skip_bits(gb, 4);  // element_instance_tag
        tags = decode_pce(avctx, m4ac, layout_map, gb);
        if (tags < 0)
            return tags;
    } else {
        if ((ret = set_default_channel_config(avctx, layout_map,
                                              &tags, channel_config)))
            return ret;
    }

    if (count_channels(layout_map, tags) > 1) {
        m4ac->ps = 0;
    } else if (m4ac->sbr == 1 && m4ac->ps == -1)
        m4ac->ps = 1;

    if (ac && (ret = output_configure(ac, layout_map, tags, OC_GLOBAL_HDR, 0)))
        return ret;

    if (extension_flag) {
        switch (m4ac->object_type) {
        case AOT_ER_BSAC:
            skip_bits(gb, 5);    // numOfSubFrame
            skip_bits(gb, 11);   // layer_length
            break;
        case AOT_ER_AAC_LC:
        case AOT_ER_AAC_LTP:
        case AOT_ER_AAC_SCALABLE:
        case AOT_ER_AAC_LD:
            res_flags = get_bits(gb, 3);
            if (res_flags) {
                avpriv_report_missing_feature(avctx,
                                              "AAC data resilience (flags %x)",
                                              res_flags);
                return AVERROR_PATCHWELCOME;
            }
            break;
        }
        skip_bits1(gb);    // extensionFlag3 (TBD in version 3)
    }
    switch (m4ac->object_type) {
    case AOT_ER_AAC_LC:
    case AOT_ER_AAC_LTP:
    case AOT_ER_AAC_SCALABLE:
    case AOT_ER_AAC_LD:
        ep_config = get_bits(gb, 2);
        if (ep_config) {
            avpriv_report_missing_feature(avctx,
                                          "epConfig %d", ep_config);
            return AVERROR_PATCHWELCOME;
        }
    }
    return 0;
}
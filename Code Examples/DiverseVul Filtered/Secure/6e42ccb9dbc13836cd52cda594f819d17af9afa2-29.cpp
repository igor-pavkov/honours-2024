static int set_default_channel_config(AVCodecContext *avctx,
                                      uint8_t (*layout_map)[3],
                                      int *tags,
                                      int channel_config)
{
    if (channel_config < 1 || channel_config > 7) {
        av_log(avctx, AV_LOG_ERROR,
               "invalid default channel configuration (%d)\n",
               channel_config);
        return AVERROR_INVALIDDATA;
    }
    *tags = tags_per_config[channel_config];
    memcpy(layout_map, aac_channel_layout_map[channel_config - 1],
           *tags * sizeof(*layout_map));

    /*
     * AAC specification has 7.1(wide) as a default layout for 8-channel streams.
     * However, at least Nero AAC encoder encodes 7.1 streams using the default
     * channel config 7, mapping the side channels of the original audio stream
     * to the second AAC_CHANNEL_FRONT pair in the AAC stream. Similarly, e.g. FAAD
     * decodes the second AAC_CHANNEL_FRONT pair as side channels, therefore decoding
     * the incorrect streams as if they were correct (and as the encoder intended).
     *
     * As actual intended 7.1(wide) streams are very rare, default to assuming a
     * 7.1 layout was intended.
     */
    if (channel_config == 7 && avctx->strict_std_compliance < FF_COMPLIANCE_STRICT) {
        av_log(avctx, AV_LOG_INFO, "Assuming an incorrectly encoded 7.1 channel layout"
               " instead of a spec-compliant 7.1(wide) layout, use -strict %d to decode"
               " according to the specification instead.\n", FF_COMPLIANCE_STRICT);
        layout_map[2][2] = AAC_CHANNEL_SIDE;
    }

    return 0;
}
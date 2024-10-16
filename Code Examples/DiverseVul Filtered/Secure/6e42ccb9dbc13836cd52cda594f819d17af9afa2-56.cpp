static int parse_adts_frame_header(AACContext *ac, GetBitContext *gb)
{
    int size;
    AACADTSHeaderInfo hdr_info;
    uint8_t layout_map[MAX_ELEM_ID*4][3];
    int layout_map_tags, ret;

    size = avpriv_aac_parse_header(gb, &hdr_info);
    if (size > 0) {
        if (!ac->warned_num_aac_frames && hdr_info.num_aac_frames != 1) {
            // This is 2 for "VLB " audio in NSV files.
            // See samples/nsv/vlb_audio.
            avpriv_report_missing_feature(ac->avctx,
                                          "More than one AAC RDB per ADTS frame");
            ac->warned_num_aac_frames = 1;
        }
        push_output_configuration(ac);
        if (hdr_info.chan_config) {
            ac->oc[1].m4ac.chan_config = hdr_info.chan_config;
            if ((ret = set_default_channel_config(ac->avctx,
                                                  layout_map,
                                                  &layout_map_tags,
                                                  hdr_info.chan_config)) < 0)
                return ret;
            if ((ret = output_configure(ac, layout_map, layout_map_tags,
                                        FFMAX(ac->oc[1].status,
                                              OC_TRIAL_FRAME), 0)) < 0)
                return ret;
        } else {
            ac->oc[1].m4ac.chan_config = 0;
            /**
             * dual mono frames in Japanese DTV can have chan_config 0
             * WITHOUT specifying PCE.
             *  thus, set dual mono as default.
             */
            if (ac->dmono_mode && ac->oc[0].status == OC_NONE) {
                layout_map_tags = 2;
                layout_map[0][0] = layout_map[1][0] = TYPE_SCE;
                layout_map[0][2] = layout_map[1][2] = AAC_CHANNEL_FRONT;
                layout_map[0][1] = 0;
                layout_map[1][1] = 1;
                if (output_configure(ac, layout_map, layout_map_tags,
                                     OC_TRIAL_FRAME, 0))
                    return -7;
            }
        }
        ac->oc[1].m4ac.sample_rate     = hdr_info.sample_rate;
        ac->oc[1].m4ac.sampling_index  = hdr_info.sampling_index;
        ac->oc[1].m4ac.object_type     = hdr_info.object_type;
        if (ac->oc[0].status != OC_LOCKED ||
            ac->oc[0].m4ac.chan_config != hdr_info.chan_config ||
            ac->oc[0].m4ac.sample_rate != hdr_info.sample_rate) {
            ac->oc[1].m4ac.sbr = -1;
            ac->oc[1].m4ac.ps  = -1;
        }
        if (!hdr_info.crc_absent)
            skip_bits(gb, 16);
    }
    return size;
}
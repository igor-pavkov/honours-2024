static int decode_extension_payload(AACContext *ac, GetBitContext *gb, int cnt,
                                    ChannelElement *che, enum RawDataBlockType elem_type)
{
    int crc_flag = 0;
    int res = cnt;
    switch (get_bits(gb, 4)) { // extension type
    case EXT_SBR_DATA_CRC:
        crc_flag++;
    case EXT_SBR_DATA:
        if (!che) {
            av_log(ac->avctx, AV_LOG_ERROR, "SBR was found before the first channel element.\n");
            return res;
        } else if (!ac->oc[1].m4ac.sbr) {
            av_log(ac->avctx, AV_LOG_ERROR, "SBR signaled to be not-present but was found in the bitstream.\n");
            skip_bits_long(gb, 8 * cnt - 4);
            return res;
        } else if (ac->oc[1].m4ac.sbr == -1 && ac->oc[1].status == OC_LOCKED) {
            av_log(ac->avctx, AV_LOG_ERROR, "Implicit SBR was found with a first occurrence after the first frame.\n");
            skip_bits_long(gb, 8 * cnt - 4);
            return res;
        } else if (ac->oc[1].m4ac.ps == -1 && ac->oc[1].status < OC_LOCKED && ac->avctx->channels == 1) {
            ac->oc[1].m4ac.sbr = 1;
            ac->oc[1].m4ac.ps = 1;
            ac->avctx->profile = FF_PROFILE_AAC_HE_V2;
            output_configure(ac, ac->oc[1].layout_map, ac->oc[1].layout_map_tags,
                             ac->oc[1].status, 1);
        } else {
            ac->oc[1].m4ac.sbr = 1;
            ac->avctx->profile = FF_PROFILE_AAC_HE;
        }
        res = ff_decode_sbr_extension(ac, &che->sbr, gb, crc_flag, cnt, elem_type);
        break;
    case EXT_DYNAMIC_RANGE:
        res = decode_dynamic_range(&ac->che_drc, gb);
        break;
    case EXT_FILL:
        decode_fill(ac, gb, 8 * cnt - 4);
        break;
    case EXT_FILL_DATA:
    case EXT_DATA_ELEMENT:
    default:
        skip_bits_long(gb, 8 * cnt - 4);
        break;
    };
    return res;
}
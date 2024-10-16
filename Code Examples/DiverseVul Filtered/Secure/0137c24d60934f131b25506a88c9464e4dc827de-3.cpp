static int dissect_dvb_s2_modeadapt(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    int         cur_off = 0, modeadapt_len, modeadapt_type, matched_headers = 0;

    proto_item *ti, *tf;
    proto_tree *dvb_s2_modeadapt_tree;
    proto_tree *dvb_s2_modeadapt_acm_tree;

    unsigned int modcod, mc;
    static int * const modeadapt_acm_bitfields[] = {
        &hf_dvb_s2_modeadapt_acm_fecframe,
        &hf_dvb_s2_modeadapt_acm_pilot,
        &hf_dvb_s2_modeadapt_acm_modcod,
        NULL
    };

    /* Check that there's enough data */
    if (tvb_captured_length(tvb) < DVB_S2_MODEADAPT_MINSIZE)
        return 0;

    /* There are four different mode adaptation formats, with different
       length headers. Two of them have a sync byte at the beginning, but
       the other two do not. In every case, the mode adaptation header is
       followed by the baseband header, which is protected by a CRC-8.
       The CRC-8 is weak protection, so it can match by accident, leading
       to an ambiguity in identifying which format is in use. We will
       check for ambiguity and report it. */
    /* Try L.1 format: no header. */
    if (test_dvb_s2_crc(tvb, DVB_S2_MODEADAPT_L1SIZE)) {
        matched_headers++;
        modeadapt_type = DVB_S2_MODEADAPT_TYPE_L1;
        modeadapt_len = DVB_S2_MODEADAPT_L1SIZE;
    }

    /* Try L.2 format: header includes sync byte */
    if ((tvb_get_guint8(tvb, DVB_S2_MODEADAPT_OFFS_SYNCBYTE) == DVB_S2_MODEADAPT_SYNCBYTE) &&
        test_dvb_s2_crc(tvb, DVB_S2_MODEADAPT_L2SIZE)) {
        matched_headers++;
        modeadapt_type = DVB_S2_MODEADAPT_TYPE_L2;
        modeadapt_len = DVB_S2_MODEADAPT_L2SIZE;
    }

    /* Try L.3 format: header includes sync byte */
    if ((tvb_get_guint8(tvb, DVB_S2_MODEADAPT_OFFS_SYNCBYTE) == DVB_S2_MODEADAPT_SYNCBYTE) &&
        test_dvb_s2_crc(tvb, DVB_S2_MODEADAPT_L3SIZE)) {
        matched_headers++;
        modeadapt_type = DVB_S2_MODEADAPT_TYPE_L3;
        modeadapt_len = DVB_S2_MODEADAPT_L3SIZE;
    }

    /* Try L.4 format: header does not include sync byte */
    if (test_dvb_s2_crc(tvb, DVB_S2_MODEADAPT_L4SIZE)) {
        matched_headers++;
        modeadapt_type = DVB_S2_MODEADAPT_TYPE_L4;
        modeadapt_len = DVB_S2_MODEADAPT_L4SIZE;
    }

    if (matched_headers == 0) {
        /* This does not look like a DVB-S2-BB frame at all. We are a
           heuristic dissector, so we should just punt and let another
           dissector have a try at this one. */
        return 0;
    }

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "DVB-S2 ");
    col_set_str(pinfo->cinfo, COL_INFO,     "DVB-S2 ");

    /* If there's a mode adaptation header, create display subtree for it */
    if (modeadapt_len > 0) {
        /* ti = proto_tree_add_item(tree, proto_dvb_s2_modeadapt, tvb, 0, modeadapt_len, ENC_NA); */
        ti = proto_tree_add_protocol_format(tree, proto_dvb_s2_modeadapt, tvb, 0, modeadapt_len,
            "DVB-S2 Mode Adaptation Header L.%d", modeadapt_type);
        dvb_s2_modeadapt_tree = proto_item_add_subtree(ti, ett_dvb_s2_modeadapt);

        if (matched_headers > 1) {
            expert_add_info_format(pinfo, ti, &ei_dvb_s2_bb_header_ambiguous,
                "Mode adaptation header format is ambiguous. Assuming L.%d", modeadapt_type);
        }

        /* SYNC byte if used in this header format; value has already been checked */
        if (modeadapt_type == DVB_S2_MODEADAPT_TYPE_L2 ||
            modeadapt_type == DVB_S2_MODEADAPT_TYPE_L3) {
            proto_tree_add_item(dvb_s2_modeadapt_tree, hf_dvb_s2_modeadapt_sync, tvb, cur_off, 1, ENC_BIG_ENDIAN);
            cur_off++;
        }

        /* ACM byte and subfields if used in this header format */
        if (modeadapt_type == DVB_S2_MODEADAPT_TYPE_L2 ||
            modeadapt_type == DVB_S2_MODEADAPT_TYPE_L3 ||
            modeadapt_type == DVB_S2_MODEADAPT_TYPE_L4) {
            mc = tvb_get_guint8(tvb, 1);
            //mc = tvb_get_letohs(tvb, 0);
            if (mc & 0x80) {
                modcod = 0x80;
                modcod |= ((mc & 0x1F) << 2);
                modcod |= ((mc & 0x40) >> 5);
                tf = proto_tree_add_item(dvb_s2_modeadapt_tree, hf_dvb_s2_modeadapt_acm, tvb,
                        DVB_S2_MODEADAPT_OFFS_ACMBYTE, 1, ENC_BIG_ENDIAN);

                dvb_s2_modeadapt_acm_tree = proto_item_add_subtree(tf, ett_dvb_s2_modeadapt_acm);

                proto_tree_add_item(dvb_s2_modeadapt_acm_tree, hf_dvb_s2_modeadapt_acm_pilot, tvb,
                        DVB_S2_MODEADAPT_OFFS_ACMBYTE, 1, ENC_BIG_ENDIAN);
                proto_tree_add_uint_format_value(dvb_s2_modeadapt_acm_tree, hf_dvb_s2_modeadapt_acm_modcod_s2x, tvb,
                        DVB_S2_MODEADAPT_OFFS_ACMBYTE, 1, mc, "DVBS2X %s(%d)", modeadapt_modcods[modcod].strptr, modcod);
            } else {
                proto_tree_add_bitmask_with_flags(dvb_s2_modeadapt_tree, tvb, DVB_S2_MODEADAPT_OFFS_ACMBYTE, hf_dvb_s2_modeadapt_acm,
                        ett_dvb_s2_modeadapt_acm, modeadapt_acm_bitfields, ENC_BIG_ENDIAN, BMT_NO_FLAGS);
            }
            cur_off++;
        }

        /* CNI and Frame No if used in this header format */
        if (modeadapt_type == DVB_S2_MODEADAPT_TYPE_L3 ||
            modeadapt_type == DVB_S2_MODEADAPT_TYPE_L4) {
            proto_tree_add_item(dvb_s2_modeadapt_tree, hf_dvb_s2_modeadapt_cni, tvb, cur_off, 1, ENC_BIG_ENDIAN);
            cur_off++;

            proto_tree_add_item(dvb_s2_modeadapt_tree, hf_dvb_s2_modeadapt_frameno, tvb, cur_off, 1, ENC_BIG_ENDIAN);
            cur_off++;
        }
    }

    /* start DVB-BB dissector */
    cur_off += dissect_dvb_s2_bb(tvb_new_subset_remaining(tvb, cur_off), pinfo, tree, NULL);

    return cur_off;
}
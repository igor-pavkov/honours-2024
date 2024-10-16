void dissect_hsdsch_common_channel_info(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
                                        int offset, struct fp_info *p_fp_info,
                                        void *data)
{
    gboolean is_control_frame;
    guint16 header_crc = 0;
    proto_item * header_crc_pi = NULL;
    guint header_length = 0;

    /* Header CRC */
    header_crc = tvb_get_bits8(tvb, 0, 7);
    header_crc_pi = proto_tree_add_item(tree, hf_fp_header_crc, tvb, offset, 1, ENC_BIG_ENDIAN);

    /* Frame Type */
    is_control_frame = tvb_get_guint8(tvb, offset) & 0x01;
    proto_tree_add_item(tree, hf_fp_ft, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset++;

    col_append_str(pinfo->cinfo, COL_INFO, is_control_frame ? " [Control] " : " [Data] ");

    if (is_control_frame) {
        dissect_common_control(tvb, pinfo, tree, offset, p_fp_info);
        /* For control frame the header CRC is actually frame CRC covering all
         * bytes except the first */
        if (preferences_header_checksum) {
            verify_control_frame_crc(tvb, pinfo, header_crc_pi, header_crc);
        }
    }
    else {
        guint8 number_of_pdu_blocks;
        gboolean drt_present = FALSE;
        gboolean fach_present = FALSE;
        guint16 user_buffer_size;
        int n;
        guint j;

        #define MAX_PDU_BLOCKS 31
        guint64 lchid[MAX_PDU_BLOCKS];
        guint64 pdu_length[MAX_PDU_BLOCKS];
        guint64 no_of_pdus[MAX_PDU_BLOCKS];
        guint8 newieflags = 0;

        umts_mac_info *macinf;
        rlc_info *rlcinf;

        rlcinf = (rlc_info *)p_get_proto_data(wmem_file_scope(), pinfo, proto_rlc, 0);
        macinf = (umts_mac_info *)p_get_proto_data(wmem_file_scope(), pinfo, proto_umts_mac, 0);
        /********************************/
        /* HS-DCH type 2 data here      */

        col_append_str(pinfo->cinfo, COL_INFO, "(ehs)");

        /* Frame Seq Nr (4 bits) */
        if ((p_fp_info->release == 6) ||
            (p_fp_info->release == 7)) {

            guint8 frame_seq_no = (tvb_get_guint8(tvb, offset) & 0xf0) >> 4;
            proto_tree_add_item(tree, hf_fp_frame_seq_nr, tvb, offset, 1, ENC_BIG_ENDIAN);

            col_append_fstr(pinfo->cinfo, COL_INFO, "  seqno=%u", frame_seq_no);
        }

        /* CmCH-PI (4 bits) */
        proto_tree_add_item(tree, hf_fp_cmch_pi, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* Total number of PDU blocks (5 bits) */
        number_of_pdu_blocks = (tvb_get_guint8(tvb, offset) >> 3);
        proto_tree_add_item(tree, hf_fp_total_pdu_blocks, tvb, offset, 1, ENC_BIG_ENDIAN);

        if (p_fp_info->release == 7) {
            /* Flush bit */
            proto_tree_add_item(tree, hf_fp_flush, tvb, offset, 1, ENC_BIG_ENDIAN);

            /* FSN/DRT reset bit */
            proto_tree_add_item(tree, hf_fp_fsn_drt_reset, tvb, offset, 1, ENC_BIG_ENDIAN);

            /* DRT Indicator */
            drt_present = tvb_get_guint8(tvb, offset) & 0x01;
            proto_tree_add_item(tree, hf_fp_drt_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
        }
        offset++;

        /* FACH Indicator flag */
        fach_present = (tvb_get_guint8(tvb, offset) & 0x80) >> 7;
        proto_tree_add_item(tree, hf_fp_fach_indicator, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset++;

        /* User buffer size */
        user_buffer_size = tvb_get_ntohs(tvb, offset);
        proto_tree_add_item(tree, hf_fp_user_buffer_size, tvb, offset, 2, ENC_BIG_ENDIAN);
        offset += 2;

        col_append_fstr(pinfo->cinfo, COL_INFO, "  User-Buffer-Size=%u", user_buffer_size);


        /********************************************************************/
        /* Now read number_of_pdu_blocks header entries                     */
        for (n=0; n < number_of_pdu_blocks; n++) {
            proto_item *pdu_block_header_ti;
            proto_tree *pdu_block_header_tree;
            int        block_header_start_offset = offset;

            /* Add PDU block header subtree */
            pdu_block_header_ti = proto_tree_add_string_format(tree, hf_fp_hsdsch_pdu_block_header,
                                                               tvb, offset, 0,
                                                               "",
                                                               "PDU Block Header");
            pdu_block_header_tree = proto_item_add_subtree(pdu_block_header_ti,
                                                           ett_fp_hsdsch_pdu_block_header);

            /* MAC-d/c PDU length in this block (11 bits) */
            proto_tree_add_bits_ret_val(pdu_block_header_tree, hf_fp_pdu_length_in_block, tvb,
                                        (offset*8) + ((n % 2) ? 4 : 0), 11,
                                        &pdu_length[n], ENC_BIG_ENDIAN);
            if ((n % 2) == 0)
                offset++;
            else
                offset += 2;


            /* # PDUs in this block (4 bits) */
            proto_tree_add_bits_ret_val(pdu_block_header_tree, hf_fp_pdus_in_block, tvb,
                                        (offset*8) + ((n % 2) ? 0 : 4), 4,
                                        &no_of_pdus[n], ENC_BIG_ENDIAN);
            if ((n % 2) == 0) {
                offset++;
            }

            /* Logical channel ID in block (4 bits) */
            proto_tree_add_bits_ret_val(pdu_block_header_tree, hf_fp_lchid, tvb,
                                        (offset*8) + ((n % 2) ? 4 : 0), 4,
                                        &lchid[n], ENC_BIG_ENDIAN);
            if ((n % 2) == 1) {
                offset++;
            }
            else {
                if (n == (number_of_pdu_blocks-1)) {
                    /* Byte is padded out for last block */
                    offset++;
                }
            }

            /* Append summary to header tree root */
            proto_item_append_text(pdu_block_header_ti,
                                   " (lch:%u, %u pdus of %u bytes)",
                                   (guint16)lchid[n],
                                   (guint16)no_of_pdus[n],
                                   (guint16)pdu_length[n]);

            /* Set length of header tree item */
            if (((n % 2) == 0) && (n < (number_of_pdu_blocks-1))) {
                proto_item_set_len(pdu_block_header_ti,
                                   offset - block_header_start_offset+1);
            }
            else {
                proto_item_set_len(pdu_block_header_ti,
                                   offset - block_header_start_offset);
            }
        }
        if (header_length == 0) {
            header_length = offset;
        }

        /**********************************************/
        /* Optional fields indicated by earlier flags */
        if (drt_present) {
            /* DRT */
            proto_tree_add_item(tree, hf_fp_drt, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;
        }

        if (fach_present) {
            /* H-RNTI: */
            proto_tree_add_item(tree, hf_fp_hrnti, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;

            /* RACH Measurement Result */
            proto_tree_add_item(tree, hf_fp_rach_measurement_result, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset++;
        }

        /********************************************************************/
        /* Now read the MAC-d/c PDUs for each block using info from headers */
        for (n=0; n < number_of_pdu_blocks; n++) {
            tvbuff_t *next_tvb;
            for (j=0; j<no_of_pdus[n]; j++) {
                /* If all bits are set, then this is BCCH or PCCH according to: 25.435 paragraph: 6.2.7.31 */
                if (lchid[n] == 0xF) {
                    /* In the very few test cases I've seen, this seems to be
                     * BCCH with transparent MAC layer. Therefore skip right to
                     * rlc_bcch and hope for the best. */
                    next_tvb = tvb_new_subset_length(tvb, offset, (gint)pdu_length[n]);
                    call_dissector_with_data(rlc_bcch_handle, next_tvb, pinfo, top_level_tree, data);
                    offset += (gint)pdu_length[n];
                } else { /* Else go for CCCH UM, this seems to work. */
                    p_fp_info->hsdsch_entity = ehs; /* HSDSCH type 2 */
                    /* TODO: use cur_tb or subnum everywhere. */
                    p_fp_info->cur_tb = j; /* set cur_tb for MAC */
                    pinfo->fd->subnum = j; /* set subframe number for RRC */
                    macinf->content[j] = MAC_CONTENT_CCCH;
                    macinf->lchid[j] = (guint8)lchid[n]+1; /*Add 1 since it is zero indexed? */
                    macinf->macdflow_id[j] = p_fp_info->hsdsch_macflowd_id;
                    macinf->ctmux[j] = FALSE;

                    rlcinf->li_size[j] = RLC_LI_7BITS;
                    rlcinf->ciphered[j] = FALSE;
                    rlcinf->deciphered[j] = FALSE;
                    rlcinf->rbid[j] = (guint8)lchid[n]+1;
                    rlcinf->urnti[j] = p_fp_info->channel; /*We need to fake urnti*/

                    next_tvb = tvb_new_subset_length(tvb, offset, (gint)pdu_length[n]);
                    call_dissector_with_data(mac_fdd_hsdsch_handle, next_tvb, pinfo, top_level_tree, data);

                    offset += (gint)pdu_length[n];
                }
            }
        }

        /* New IE Flags */
        newieflags = tvb_get_guint8(tvb, offset);
        /* If newieflags == 0000 0010 then this indicates that there is a
         * HS-DSCH physical layer category and no other New IE flags. */
        if (newieflags == 2) {
            /* HS-DSCH physical layer category presence bit. */
            proto_tree_add_uint(tree, hf_fp_hsdsch_new_ie_flag[6], tvb, offset, 1, newieflags);
            offset++;
            /* HS-DSCH physical layer category. */
            proto_tree_add_bits_item(tree, hf_fp_hsdsch_physical_layer_category, tvb, offset*8, 6, ENC_BIG_ENDIAN);
            offset++;
        }
        if (preferences_header_checksum) {
            verify_header_crc(tvb, pinfo, header_crc_pi, header_crc, header_length);
        }
        /* Spare Extension and Payload CRC */
        dissect_spare_extension_and_crc(tvb, pinfo, tree, 1, offset, header_length);
    }
}
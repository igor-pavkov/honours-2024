void proto_register_dvb_s2_modeadapt(void)
{
    module_t *dvb_s2_modeadapt_module;

    static hf_register_info hf_modeadapt[] = {
        {&hf_dvb_s2_modeadapt_sync, {
                "Sync Byte", "dvb-s2_modeadapt.sync",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "Das Sync Byte", HFILL}
        },
        {&hf_dvb_s2_modeadapt_acm, {
                "ACM command", "dvb-s2_modeadapt.acmcmd",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_modeadapt_acm_fecframe, {
                "FEC frame size", "dvb-s2_modeadapt.acmcmd.fecframe",
                FT_BOOLEAN, 8, TFS(&tfs_modeadapt_fecframe), DVB_S2_MODEADAPT_FECFRAME_MASK,
                "FEC", HFILL}
        },
        {&hf_dvb_s2_modeadapt_acm_pilot, {
                "Pilots configuration", "dvb-s2_modeadapt.acmcmd.pilots",
                FT_BOOLEAN, 8, TFS(&tfs_modeadapt_pilots), DVB_S2_MODEADAPT_PILOTS_MASK,
                "Pilots", HFILL}
        },
        {&hf_dvb_s2_modeadapt_acm_modcod, {
                "Modcod indicator", "dvb-s2_modeadapt.acmcmd.modcod",
                FT_UINT8, BASE_DEC|BASE_EXT_STRING, &modeadapt_modcods_ext, DVB_S2_MODEADAPT_MODCODS_MASK,
                "Modcod", HFILL}
        },
        {&hf_dvb_s2_modeadapt_acm_modcod_s2x, {
                "Modcod indicator", "dvb-s2_modeadapt.acmcmd.modcod",
                FT_UINT8, BASE_DEC|BASE_EXT_STRING, &modeadapt_modcods_ext, DVB_S2_MODEADAPT_MODCODS_S2X_MASK,
                "Modcod S2X", HFILL}
        },
        {&hf_dvb_s2_modeadapt_cni, {
                "Carrier to Noise [dB]", "dvb-s2_modeadapt.cni",
                FT_UINT8, BASE_DEC|BASE_EXT_STRING, &modeadapt_esno_ext, 0x0,
                "CNI", HFILL}
        },
        {&hf_dvb_s2_modeadapt_frameno, {
                "Frame number", "dvb-s2_modeadapt.frameno",
                FT_UINT8, BASE_DEC, NULL, 0x0,
                "fno", HFILL}
        }
    };

/* Setup protocol subtree array */
    static gint *ett_modeadapt[] = {
        &ett_dvb_s2_modeadapt,
        &ett_dvb_s2_modeadapt_acm
    };

    static hf_register_info hf_bb[] = {
        {&hf_dvb_s2_bb_matype1, {
                "MATYPE1", "dvb-s2_bb.matype1",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "MATYPE1 Header Field", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_gs, {
                "TS/GS Stream Input", "dvb-s2_bb.matype1.tsgs",
                FT_UINT8, BASE_DEC, VALS(bb_tsgs), DVB_S2_BB_TSGS_MASK,
                "Transport Stream Input or Generic Stream Input", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_mis, {
                "Input Stream", "dvb-s2_bb.matype1.mis",
                FT_BOOLEAN, 8, TFS(&tfs_bb_mis), DVB_S2_BB_MIS_MASK,
                "Single Input Stream or Multiple Input Stream", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_acm, {
                "Coding and Modulation", "dvb-s2_bb.matype1.acm",
                FT_BOOLEAN, 8, TFS(&tfs_bb_acm), DVB_S2_BB_ACM_MASK,
                "Constant Coding and Modulation or Adaptive Coding and Modulation", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_issyi, {
                "ISSYI", "dvb-s2_bb.matype1.issyi",
                FT_BOOLEAN, 8, TFS(&tfs_bb_issyi), DVB_S2_BB_ISSYI_MASK,
                "Input Stream Synchronization Indicator", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_npd, {
                "NPD", "dvb-s2_bb.matype1.npd",
                FT_BOOLEAN, 8, TFS(&tfs_bb_npd), DVB_S2_BB_NPD_MASK,
                "Null-packet deletion enabled", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_high_ro, {
                "RO", "dvb-s2_bb.matype1.ro",
                FT_UINT8, BASE_DEC, VALS(bb_high_ro), DVB_S2_BB_RO_MASK,
                "Transmission Roll-off factor", HFILL}
        },
        {&hf_dvb_s2_bb_matype1_low_ro, {
                "RO", "dvb-s2_bb.matype1.ro",
                FT_UINT8, BASE_DEC, VALS(bb_low_ro), DVB_S2_BB_RO_MASK,
                "Transmission Roll-off factor", HFILL}
        },
        {&hf_dvb_s2_bb_matype2, {
                "MATYPE2", "dvb-s2_bb.matype2",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "MATYPE2 Header Field", HFILL}
        },
        {&hf_dvb_s2_bb_upl, {
                "UPL", "dvb-s2_bb.upl",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                "User Packet Length", HFILL}
        },
        {&hf_dvb_s2_bb_dfl, {
                "DFL", "dvb-s2_bb.dfl",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                "Data Field Length", HFILL}
        },
        {&hf_dvb_s2_bb_sync, {
                "SYNC", "dvb-s2_bb.sync",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "Copy of the User Packet Sync-byte", HFILL}
        },
        {&hf_dvb_s2_bb_syncd, {
                "SYNCD", "dvb-s2_bb.syncd",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                "Distance to first user packet", HFILL}
        },
        {&hf_dvb_s2_bb_crc, {
                "Checksum", "dvb-s2_bb.crc",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "BB Header CRC-8", HFILL}
        },
        {&hf_dvb_s2_bb_crc_status, {
                "Checksum Status", "dvb-s2_bb.crc.status",
                FT_UINT8, BASE_NONE, VALS(proto_checksum_vals), 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_bb_packetized, {
                "Packetized Generic Stream Data", "dvb-s2_bb.packetized",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "Packetized Generic Stream (non-TS) Data", HFILL}
        },
        {&hf_dvb_s2_bb_transport, {
                "Transport Stream Data", "dvb-s2_bb.transport",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "Transport Stream (TS) Data", HFILL}
        },
        {&hf_dvb_s2_bb_reserved, {
                "Reserved Stream Type Data", "dvb-s2_bb.reserved",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "Stream of an unknown reserved type", HFILL}
        },
        {&hf_dvb_s2_bb_df, {
                "BBFrame user data", "dvb-s2_bb.df",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL}
        },
        {&hf_dvb_s2_bb_eip_crc32, {
                "EIP CRC32", "dvb-s2_bb.eip_crc32",
                FT_UINT32, BASE_HEX, NULL, 0x0,
                "Explicit Integrity Protection CRC32", HFILL}
        }
    };

    static gint *ett_bb[] = {
        &ett_dvb_s2_bb,
        &ett_dvb_s2_bb_matype1
    };

    /* DVB-S2 GSE Frame */
    static hf_register_info hf_gse[] = {
        {&hf_dvb_s2_gse_hdr, {
                "GSE header", "dvb-s2_gse.hdr",
                FT_UINT16, BASE_HEX, NULL, 0x0,
                "GSE Header (start/stop/length)", HFILL}
        },
        {&hf_dvb_s2_gse_hdr_start, {
                "Start", "dvb-s2_gse.hdr.start",
                FT_BOOLEAN, 16, TFS(&tfs_enabled_disabled), DVB_S2_GSE_HDR_START_MASK,
                "Start Indicator", HFILL}
        },
        {&hf_dvb_s2_gse_hdr_stop, {
                "Stop", "dvb-s2_gse.hdr.stop",
                FT_BOOLEAN, 16, TFS(&tfs_enabled_disabled), DVB_S2_GSE_HDR_STOP_MASK,
                "Stop Indicator", HFILL}
        },
        {&hf_dvb_s2_gse_hdr_labeltype, {
                "Label Type", "dvb-s2_gse.hdr.labeltype",
                FT_UINT16, BASE_HEX, VALS(gse_labeltype), DVB_S2_GSE_HDR_LABELTYPE_MASK,
                "Label Type Indicator", HFILL}
        },
        {&hf_dvb_s2_gse_hdr_length, {
                "Length", "dvb-s2_gse.hdr.length",
                FT_UINT16, BASE_DEC, NULL, DVB_S2_GSE_HDR_LENGTH_MASK,
                "GSE Length", HFILL}
        },
        {&hf_dvb_s2_gse_padding, {
                "GSE Padding", "dvb-s2_gse.padding",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                "GSE Padding Bytes", HFILL}
        },
        {&hf_dvb_s2_gse_proto_next_header, {
                "Protocol", "dvb-s2_gse.proto",
                FT_UINT16, BASE_HEX, VALS(gse_proto_next_header_str), 0x0,
                "Protocol Type", HFILL}
        },
        {&hf_dvb_s2_gse_proto_ethertype, {
                "Protocol", "dvb-s2_gse.proto",
                FT_UINT16, BASE_HEX, VALS(etype_vals), 0x0,
                "Protocol Type", HFILL}
        },
        {&hf_dvb_s2_gse_label6, {
                "Label", "dvb-s2_gse.label_ether",
                FT_ETHER, BASE_NONE, NULL, 0x0,
                "Label Field", HFILL}
        },
        {&hf_dvb_s2_gse_label3, {
                "Label", "dvb-s2_gse.label",
                FT_UINT24, BASE_HEX, NULL, 0x0,
                "Label Field", HFILL}
        },
        {&hf_dvb_s2_gse_fragid, {
                "Frag ID", "dvb-s2_gse.fragid",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "Fragment ID", HFILL}
        },
        {&hf_dvb_s2_gse_totlength, {
                "Total Length", "dvb-s2_gse.totlength",
                FT_UINT16, BASE_DEC, NULL, 0x0,
                "GSE Total Frame Length", HFILL}
        },
        {&hf_dvb_s2_gse_exthdr, {
                "Extension Header", "dvb-s2_gse.exthdr",
                FT_UINT8, BASE_HEX, NULL, 0x0,
                "optional Extension Header", HFILL}
        },
        {&hf_dvb_s2_gse_ncr, {
                "NCR Packet", "dvb-s2_gse.ncr",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "GSE NCR PAcket", HFILL}
        },
        {&hf_dvb_s2_gse_data, {
                "PDU Data", "dvb-s2_gse.data",
                FT_BYTES, BASE_NONE, NULL, 0x0,
                "GSE Frame User Data", HFILL}
        },
        {&hf_dvb_s2_gse_crc32, {
                "CRC", "dvb-s2_gse.crc",
                FT_UINT32, BASE_HEX, NULL, 0x0,
                "CRC-32", HFILL}
        },
        { &hf_dvbs2_fragment_overlap,
            { "Fragment overlap", "dvb-s2_gse.fragment.overlap", FT_BOOLEAN, BASE_NONE,
                NULL, 0x0, "Fragment overlaps with other fragments", HFILL }},

        { &hf_dvbs2_fragment_overlap_conflict,
            { "Conflicting data in fragment overlap", "dvb-s2_gse.fragment.overlap.conflict",
                FT_BOOLEAN, BASE_NONE, NULL, 0x0,
                "Overlapping fragments contained conflicting data", HFILL }},

        { &hf_dvbs2_fragment_multiple_tails,
            { "Multiple tail fragments found", "dvb-s2_gse.fragment.multipletails",
                FT_BOOLEAN, BASE_NONE, NULL, 0x0,
                "Several tails were found when defragmenting the packet", HFILL }},

        { &hf_dvbs2_fragment_too_long_fragment,
            { "Fragment too long", "dvb-s2_gse.fragment.toolongfragment",
                FT_BOOLEAN, BASE_NONE, NULL, 0x0,
                "Fragment contained data past end of packet", HFILL }},

        { &hf_dvbs2_fragment_error,
            { "Defragmentation error", "dvb-s2_gse.fragment.error", FT_FRAMENUM, BASE_NONE,
                NULL, 0x0, "Defragmentation error due to illegal fragments", HFILL }},

        { &hf_dvbs2_fragment_count,
            { "Fragment count", "dvb-s2_gse.fragment.count", FT_UINT32, BASE_DEC,
                NULL, 0x0, NULL, HFILL }},

        { &hf_dvbs2_fragment,
            { "DVB-S2 GSE Fragment", "dvb-s2_gse.fragment", FT_FRAMENUM, BASE_NONE,
                NULL, 0x0, NULL, HFILL }},

        { &hf_dvbs2_fragments,
            { "DVB-S2 GSE Fragments", "dvb-s2_gse.fragments", FT_BYTES, BASE_NONE,
                NULL, 0x0, NULL, HFILL }},

        { &hf_dvbs2_reassembled_in,
            { "Reassembled DVB-S2 GSE in frame", "dvb-s2_gse.reassembled_in", FT_FRAMENUM, BASE_NONE,
                NULL, 0x0, "This GSE packet is reassembled in this frame", HFILL }},

        { &hf_dvbs2_reassembled_length,
            { "Reassembled DVB-S2 GSE length", "dvb-s2_gse.reassembled.length", FT_UINT32, BASE_DEC,
                NULL, 0x0, "The total length of the reassembled payload", HFILL }},

        { &hf_dvbs2_reassembled_data,
            { "Reassembled DVB-S2 GSE data", "dvb-s2_gse.reassembled.data", FT_BYTES, BASE_NONE,
                NULL, 0x0, "The reassembled payload", HFILL }}
    };

    static gint *ett_gse[] = {
        &ett_dvb_s2_gse,
        &ett_dvb_s2_gse_hdr,
        &ett_dvb_s2_gse_ncr,
        &ett_dvbs2_fragments,
        &ett_dvbs2_fragment,
    };

    static ei_register_info ei[] = {
        { &ei_dvb_s2_bb_crc, { "dvb-s2_bb.bad_checksum", PI_CHECKSUM, PI_ERROR, "Bad checksum", EXPFILL }},
        { &ei_dvb_s2_bb_issy_invalid, {"dvb-s2_bb.issy_invalid", PI_PROTOCOL, PI_WARN, "ISSY is active, which is not allowed for GSE packets", EXPFILL }},
        { &ei_dvb_s2_bb_npd_invalid, {"dvb-s2_bb.npd_invalid", PI_PROTOCOL, PI_WARN, "NPD is active, which is not allowed for GSE packets", EXPFILL }},
        { &ei_dvb_s2_bb_upl_invalid, {"dvb-s2_bb.upl_invalid", PI_PROTOCOL, PI_WARN, "User Packet Length non-zero, which is not allowed for GSE packets", EXPFILL }},
        { &ei_dvb_s2_bb_reserved, {"dvb-s2_bb.reserved_frame_format", PI_PROTOCOL, PI_WARN, "Reserved frame format in TS/GS is not defined", EXPFILL }},
        { &ei_dvb_s2_bb_header_ambiguous, { "dvb-s2_bb.header_ambiguous", PI_ASSUMPTION, PI_WARN, "Mode Adaptation header ambiguous", EXPFILL }},
    };

    expert_module_t* expert_dvb_s2_bb;

    proto_dvb_s2_modeadapt = proto_register_protocol("DVB-S2 Mode Adaptation Header", "DVB-S2", "dvb-s2_modeadapt");

    proto_dvb_s2_bb = proto_register_protocol("DVB-S2 Baseband Frame", "DVB-S2-BB", "dvb-s2_bb");

    proto_dvb_s2_gse = proto_register_protocol("DVB-S2 GSE Packet", "DVB-S2-GSE", "dvb-s2_gse");

    proto_register_field_array(proto_dvb_s2_modeadapt, hf_modeadapt, array_length(hf_modeadapt));
    proto_register_subtree_array(ett_modeadapt, array_length(ett_modeadapt));

    proto_register_field_array(proto_dvb_s2_bb, hf_bb, array_length(hf_bb));
    proto_register_subtree_array(ett_bb, array_length(ett_bb));
    expert_dvb_s2_bb = expert_register_protocol(proto_dvb_s2_bb);
    expert_register_field_array(expert_dvb_s2_bb, ei, array_length(ei));

    proto_register_field_array(proto_dvb_s2_gse, hf_gse, array_length(hf_gse));
    proto_register_subtree_array(ett_gse, array_length(ett_gse));

    dvb_s2_modeadapt_module = prefs_register_protocol(proto_dvb_s2_modeadapt, proto_reg_handoff_dvb_s2_modeadapt);

    prefs_register_obsolete_preference(dvb_s2_modeadapt_module, "enable");

    prefs_register_bool_preference(dvb_s2_modeadapt_module, "decode_df",
        "Enable dissection of DATA FIELD",
        "Check this to enable full protocol dissection of data above BBHeader",
        &dvb_s2_df_dissection);

    prefs_register_bool_preference(dvb_s2_modeadapt_module, "full_decode",
        "Enable dissection of GSE data",
        "Check this to enable full protocol dissection of data above GSE Layer",
        &dvb_s2_full_dissection);

    register_init_routine(dvbs2_defragment_init);
}
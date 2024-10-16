void proto_reg_handoff_dvb_s2_modeadapt(void)
{
    static gboolean prefs_initialized = FALSE;

    if (!prefs_initialized) {
        heur_dissector_add("udp", dissect_dvb_s2_modeadapt, "DVB-S2 over UDP", "dvb_s2_udp", proto_dvb_s2_modeadapt, HEURISTIC_DISABLE);
        ip_handle   = find_dissector_add_dependency("ip", proto_dvb_s2_bb);
        ipv6_handle = find_dissector_add_dependency("ipv6", proto_dvb_s2_bb);
        dvb_s2_table_handle = find_dissector("dvb-s2_table");
        eth_withoutfcs_handle = find_dissector("eth_withoutfcs");
        data_handle = find_dissector("data");
        prefs_initialized = TRUE;
    }
}
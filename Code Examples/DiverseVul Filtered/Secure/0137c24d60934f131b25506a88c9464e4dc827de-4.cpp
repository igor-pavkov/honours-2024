static gboolean test_dvb_s2_crc(tvbuff_t *tvb, guint offset) {

    guint8 input8;

    /* only check BB Header and return */
    if (tvb_captured_length(tvb) < (offset + DVB_S2_BB_HEADER_LEN))
        return FALSE;

    input8 = tvb_get_guint8(tvb, offset + DVB_S2_BB_OFFS_CRC);

    if (compute_crc8(tvb, DVB_S2_BB_HEADER_LEN - 1, offset) != input8)
        return FALSE;
    else
        return TRUE;
}
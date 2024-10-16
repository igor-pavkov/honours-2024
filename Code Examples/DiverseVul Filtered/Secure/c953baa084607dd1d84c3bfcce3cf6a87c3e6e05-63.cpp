static uint32_t get_sgpd_sync_index(const MOVStreamContext *sc, int nal_unit_type)
{
    for (uint32_t i = 0; i < sc->sgpd_sync_count; i++)
        if (sc->sgpd_sync[i] == HEVC_NAL_CRA_NUT)
            return i + 1;
    return 0;
}
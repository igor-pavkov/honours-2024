static void vhdx_update_bat_table_entry(BlockDriverState *bs, BDRVVHDXState *s,
                                       VHDXSectorInfo *sinfo,
                                       uint64_t *bat_entry_le,
                                       uint64_t *bat_offset, int state)
{
    /* The BAT entry is a uint64, with 44 bits for the file offset in units of
     * 1MB, and 3 bits for the block state. */
    s->bat[sinfo->bat_idx]  = sinfo->file_offset;

    s->bat[sinfo->bat_idx] |= state & VHDX_BAT_STATE_BIT_MASK;

    *bat_entry_le = cpu_to_le64(s->bat[sinfo->bat_idx]);
    *bat_offset = s->bat_offset + sinfo->bat_idx * sizeof(VHDXBatEntry);

}
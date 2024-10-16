static void vhdx_block_translate(BDRVVHDXState *s, int64_t sector_num,
                                 int nb_sectors, VHDXSectorInfo *sinfo)
{
    uint32_t block_offset;

    sinfo->bat_idx = sector_num >> s->sectors_per_block_bits;
    /* effectively a modulo - this gives us the offset into the block
     * (in sector sizes) for our sector number */
    block_offset = sector_num - (sinfo->bat_idx << s->sectors_per_block_bits);
    /* the chunk ratio gives us the interleaving of the sector
     * bitmaps, so we need to advance our page block index by the
     * sector bitmaps entry number */
    sinfo->bat_idx += sinfo->bat_idx >> s->chunk_ratio_bits;

    /* the number of sectors we can read/write in this cycle */
    sinfo->sectors_avail = s->sectors_per_block - block_offset;

    sinfo->bytes_left = sinfo->sectors_avail << s->logical_sector_size_bits;

    if (sinfo->sectors_avail > nb_sectors) {
        sinfo->sectors_avail = nb_sectors;
    }

    sinfo->bytes_avail = sinfo->sectors_avail << s->logical_sector_size_bits;

    sinfo->file_offset = s->bat[sinfo->bat_idx] & VHDX_BAT_FILE_OFF_MASK;

    sinfo->block_offset = block_offset << s->logical_sector_size_bits;

    /* The file offset must be past the header section, so must be > 0 */
    if (sinfo->file_offset == 0) {
        return;
    }

    /* block offset is the offset in vhdx logical sectors, in
     * the payload data block. Convert that to a byte offset
     * in the block, and add in the payload data block offset
     * in the file, in bytes, to get the final read address */

    sinfo->file_offset += sinfo->block_offset;
}
static void vhdx_set_shift_bits(BDRVVHDXState *s)
{
    s->logical_sector_size_bits = 31 - clz32(s->logical_sector_size);
    s->sectors_per_block_bits =   31 - clz32(s->sectors_per_block);
    s->chunk_ratio_bits =         63 - clz64(s->chunk_ratio);
    s->block_size_bits =          31 - clz32(s->block_size);
}
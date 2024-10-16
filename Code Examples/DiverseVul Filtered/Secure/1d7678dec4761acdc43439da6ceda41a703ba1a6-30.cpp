static void vhdx_calc_bat_entries(BDRVVHDXState *s)
{
    uint32_t data_blocks_cnt, bitmap_blocks_cnt;

    data_blocks_cnt = s->virtual_disk_size >> s->block_size_bits;
    if (s->virtual_disk_size - (data_blocks_cnt << s->block_size_bits)) {
        data_blocks_cnt++;
    }
    bitmap_blocks_cnt = data_blocks_cnt >> s->chunk_ratio_bits;
    if (data_blocks_cnt - (bitmap_blocks_cnt << s->chunk_ratio_bits)) {
        bitmap_blocks_cnt++;
    }

    if (s->parent_entries) {
        s->bat_entries = bitmap_blocks_cnt * (s->chunk_ratio + 1);
    } else {
        s->bat_entries = data_blocks_cnt +
                         ((data_blocks_cnt - 1) >> s->chunk_ratio_bits);
    }

}
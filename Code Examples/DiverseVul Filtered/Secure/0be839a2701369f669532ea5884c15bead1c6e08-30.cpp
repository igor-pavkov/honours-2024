static int ram_find_and_save_block(QEMUFile *f, bool last_stage)
{
    RAMBlock *block = last_seen_block;
    ram_addr_t offset = last_offset;
    bool complete_round = false;
    int bytes_sent = 0;
    MemoryRegion *mr;

    if (!block)
        block = QTAILQ_FIRST(&ram_list.blocks);

    while (true) {
        mr = block->mr;
        offset = migration_bitmap_find_and_reset_dirty(mr, offset);
        if (complete_round && block == last_seen_block &&
            offset >= last_offset) {
            break;
        }
        if (offset >= block->length) {
            offset = 0;
            block = QTAILQ_NEXT(block, next);
            if (!block) {
                block = QTAILQ_FIRST(&ram_list.blocks);
                complete_round = true;
                ram_bulk_stage = false;
            }
        } else {
            bytes_sent = ram_save_page(f, block, offset, last_stage);

            /* if page is unmodified, continue to the next */
            if (bytes_sent > 0) {
                last_sent_block = block;
                break;
            }
        }
    }
    last_seen_block = block;
    last_offset = offset;

    return bytes_sent;
}
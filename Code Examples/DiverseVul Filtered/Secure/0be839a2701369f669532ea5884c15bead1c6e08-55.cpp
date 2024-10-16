uint64_t ram_bytes_total(void)
{
    RAMBlock *block;
    uint64_t total = 0;

    QTAILQ_FOREACH(block, &ram_list.blocks, next)
        total += block->length;

    return total;
}
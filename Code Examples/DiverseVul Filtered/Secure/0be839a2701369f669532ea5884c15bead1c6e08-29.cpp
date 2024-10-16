static int ram_save_page(QEMUFile *f, RAMBlock* block, ram_addr_t offset,
                         bool last_stage)
{
    int bytes_sent;
    int cont;
    ram_addr_t current_addr;
    MemoryRegion *mr = block->mr;
    uint8_t *p;
    int ret;
    bool send_async = true;

    cont = (block == last_sent_block) ? RAM_SAVE_FLAG_CONTINUE : 0;

    p = memory_region_get_ram_ptr(mr) + offset;

    /* In doubt sent page as normal */
    bytes_sent = -1;
    ret = ram_control_save_page(f, block->offset,
                           offset, TARGET_PAGE_SIZE, &bytes_sent);

    XBZRLE_cache_lock();

    current_addr = block->offset + offset;
    if (ret != RAM_SAVE_CONTROL_NOT_SUPP) {
        if (ret != RAM_SAVE_CONTROL_DELAYED) {
            if (bytes_sent > 0) {
                acct_info.norm_pages++;
            } else if (bytes_sent == 0) {
                acct_info.dup_pages++;
            }
        }
    } else if (is_zero_range(p, TARGET_PAGE_SIZE)) {
        acct_info.dup_pages++;
        bytes_sent = save_block_hdr(f, block, offset, cont,
                                    RAM_SAVE_FLAG_COMPRESS);
        qemu_put_byte(f, 0);
        bytes_sent++;
        /* Must let xbzrle know, otherwise a previous (now 0'd) cached
         * page would be stale
         */
        xbzrle_cache_zero_page(current_addr);
    } else if (!ram_bulk_stage && migrate_use_xbzrle()) {
        bytes_sent = save_xbzrle_page(f, &p, current_addr, block,
                                      offset, cont, last_stage);
        if (!last_stage) {
            /* Can't send this cached data async, since the cache page
             * might get updated before it gets to the wire
             */
            send_async = false;
        }
    }

    /* XBZRLE overflow or normal page */
    if (bytes_sent == -1) {
        bytes_sent = save_block_hdr(f, block, offset, cont, RAM_SAVE_FLAG_PAGE);
        if (send_async) {
            qemu_put_buffer_async(f, p, TARGET_PAGE_SIZE);
        } else {
            qemu_put_buffer(f, p, TARGET_PAGE_SIZE);
        }
        bytes_sent += TARGET_PAGE_SIZE;
        acct_info.norm_pages++;
    }

    XBZRLE_cache_unlock();

    return bytes_sent;
}
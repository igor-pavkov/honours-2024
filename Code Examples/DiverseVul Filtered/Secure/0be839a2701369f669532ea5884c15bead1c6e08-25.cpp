static int ram_load(QEMUFile *f, void *opaque, int version_id)
{
    int flags = 0, ret = 0;
    static uint64_t seq_iter;

    seq_iter++;

    if (version_id != 4) {
        ret = -EINVAL;
    }

    while (!ret && !(flags & RAM_SAVE_FLAG_EOS)) {
        ram_addr_t addr, total_ram_bytes;
        void *host;
        uint8_t ch;

        addr = qemu_get_be64(f);
        flags = addr & ~TARGET_PAGE_MASK;
        addr &= TARGET_PAGE_MASK;

        switch (flags & ~RAM_SAVE_FLAG_CONTINUE) {
        case RAM_SAVE_FLAG_MEM_SIZE:
            /* Synchronize RAM block list */
            total_ram_bytes = addr;
            while (!ret && total_ram_bytes) {
                RAMBlock *block;
                uint8_t len;
                char id[256];
                ram_addr_t length;

                len = qemu_get_byte(f);
                qemu_get_buffer(f, (uint8_t *)id, len);
                id[len] = 0;
                length = qemu_get_be64(f);

                QTAILQ_FOREACH(block, &ram_list.blocks, next) {
                    if (!strncmp(id, block->idstr, sizeof(id))) {
                        if (block->length != length) {
                            error_report("Length mismatch: %s: 0x" RAM_ADDR_FMT
                                         " in != 0x" RAM_ADDR_FMT, id, length,
                                         block->length);
                            ret =  -EINVAL;
                        }
                        break;
                    }
                }

                if (!block) {
                    error_report("Unknown ramblock \"%s\", cannot "
                                 "accept migration", id);
                    ret = -EINVAL;
                }

                total_ram_bytes -= length;
            }
            break;
        case RAM_SAVE_FLAG_COMPRESS:
            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Illegal RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }

            ch = qemu_get_byte(f);
            ram_handle_compressed(host, ch, TARGET_PAGE_SIZE);
            break;
        case RAM_SAVE_FLAG_PAGE:
            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Illegal RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }

            qemu_get_buffer(f, host, TARGET_PAGE_SIZE);
            break;
        case RAM_SAVE_FLAG_XBZRLE:
            host = host_from_stream_offset(f, addr, flags);
            if (!host) {
                error_report("Illegal RAM offset " RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }

            if (load_xbzrle(f, addr, host) < 0) {
                error_report("Failed to decompress XBZRLE page at "
                             RAM_ADDR_FMT, addr);
                ret = -EINVAL;
                break;
            }
            break;
        case RAM_SAVE_FLAG_EOS:
            /* normal exit */
            break;
        default:
            if (flags & RAM_SAVE_FLAG_HOOK) {
                ram_control_load_hook(f, flags);
            } else {
                error_report("Unknown combination of migration flags: %#x",
                             flags);
                ret = -EINVAL;
            }
        }
        if (!ret) {
            ret = qemu_file_get_error(f);
        }
    }

    DPRINTF("Completed load of VM with exit code %d seq iteration "
            "%" PRIu64 "\n", ret, seq_iter);
    return ret;
}
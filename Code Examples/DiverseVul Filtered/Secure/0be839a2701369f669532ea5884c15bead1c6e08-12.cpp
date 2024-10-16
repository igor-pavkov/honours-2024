static size_t save_block_hdr(QEMUFile *f, RAMBlock *block, ram_addr_t offset,
                             int cont, int flag)
{
    size_t size;

    qemu_put_be64(f, offset | cont | flag);
    size = 8;

    if (!cont) {
        qemu_put_byte(f, strlen(block->idstr));
        qemu_put_buffer(f, (uint8_t *)block->idstr,
                        strlen(block->idstr));
        size += 1 + strlen(block->idstr);
    }
    return size;
}
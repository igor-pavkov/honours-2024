static int get_unused_buffer(QEMUFile *f, void *pv, size_t size)
{
    uint8_t buf[1024];
    int block_len;

    while (size > 0) {
        block_len = MIN(sizeof(buf), size);
        size -= block_len;
        qemu_get_buffer(f, buf, block_len);
    }
   return 0;
}
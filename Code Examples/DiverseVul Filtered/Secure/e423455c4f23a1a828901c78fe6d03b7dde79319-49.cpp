ssize_t read_targphys(const char *name,
                      int fd, hwaddr dst_addr, size_t nbytes)
{
    uint8_t *buf;
    ssize_t did;

    buf = g_malloc(nbytes);
    did = read(fd, buf, nbytes);
    if (did > 0)
        rom_add_blob_fixed("read", buf, did, dst_addr);
    g_free(buf);
    return did;
}
int load_image_gzipped(const char *filename, hwaddr addr, uint64_t max_sz)
{
    int bytes;
    uint8_t *data;

    bytes = load_image_gzipped_buffer(filename, max_sz, &data);
    if (bytes != -1) {
        rom_add_blob_fixed(filename, data, bytes, addr);
        g_free(data);
    }
    return bytes;
}
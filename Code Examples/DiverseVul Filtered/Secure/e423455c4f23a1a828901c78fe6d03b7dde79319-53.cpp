int load_targphys_hex_as(const char *filename, hwaddr *entry, AddressSpace *as)
{
    gsize hex_blob_size;
    gchar *hex_blob;
    int total_size = 0;

    if (!g_file_get_contents(filename, &hex_blob, &hex_blob_size, NULL)) {
        return -1;
    }

    total_size = parse_hex_blob(filename, entry, (uint8_t *)hex_blob,
                                hex_blob_size, as);

    g_free(hex_blob);
    return total_size;
}
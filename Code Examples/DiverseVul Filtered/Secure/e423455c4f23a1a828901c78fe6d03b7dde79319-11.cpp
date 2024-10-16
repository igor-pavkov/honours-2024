int load_image_targphys_as(const char *filename,
                           hwaddr addr, uint64_t max_sz, AddressSpace *as)
{
    int size;

    size = get_image_size(filename);
    if (size < 0 || size > max_sz) {
        return -1;
    }
    if (size > 0) {
        if (rom_add_file_fixed_as(filename, addr, -1, as) < 0) {
            return -1;
        }
    }
    return size;
}
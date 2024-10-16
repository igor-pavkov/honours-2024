int load_image_mr(const char *filename, MemoryRegion *mr)
{
    int size;

    if (!memory_access_is_direct(mr, false)) {
        /* Can only load an image into RAM or ROM */
        return -1;
    }

    size = get_image_size(filename);

    if (size < 0 || size > memory_region_size(mr)) {
        return -1;
    }
    if (size > 0) {
        if (rom_add_file_mr(filename, mr, -1) < 0) {
            return -1;
        }
    }
    return size;
}
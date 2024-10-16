int load_ramdisk(const char *filename, hwaddr addr, uint64_t max_sz)
{
    return load_ramdisk_as(filename, addr, max_sz, NULL);
}
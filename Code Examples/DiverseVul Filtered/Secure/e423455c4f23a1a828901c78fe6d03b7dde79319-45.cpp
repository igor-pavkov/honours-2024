int load_image_targphys(const char *filename,
                        hwaddr addr, uint64_t max_sz)
{
    return load_image_targphys_as(filename, addr, max_sz, NULL);
}
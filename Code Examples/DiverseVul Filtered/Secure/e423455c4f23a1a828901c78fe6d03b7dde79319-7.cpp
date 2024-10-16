int load_ramdisk_as(const char *filename, hwaddr addr, uint64_t max_sz,
                    AddressSpace *as)
{
    return load_uboot_image(filename, NULL, &addr, NULL, IH_TYPE_RAMDISK,
                            NULL, NULL, as);
}
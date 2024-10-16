int load_uimage(const char *filename, hwaddr *ep, hwaddr *loadaddr,
                int *is_linux,
                uint64_t (*translate_fn)(void *, uint64_t),
                void *translate_opaque)
{
    return load_uboot_image(filename, ep, loadaddr, is_linux, IH_TYPE_KERNEL,
                            translate_fn, translate_opaque, NULL);
}
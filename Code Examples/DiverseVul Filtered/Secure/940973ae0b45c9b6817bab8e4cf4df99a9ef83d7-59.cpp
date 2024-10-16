static int ide_drive_post_load(void *opaque, int version_id)
{
    IDEState *s = opaque;

    if (s->identify_set) {
        bdrv_set_enable_write_cache(s->bs, !!(s->identify_data[85] & (1 << 5)));
    }
    return 0;
}
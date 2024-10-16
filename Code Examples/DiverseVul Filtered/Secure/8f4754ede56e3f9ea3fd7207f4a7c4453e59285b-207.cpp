void bdrv_get_full_backing_filename(BlockDriverState *bs, char *dest, size_t sz)
{
    if (bs->backing_file[0] == '\0' || path_has_protocol(bs->backing_file)) {
        pstrcpy(dest, sz, bs->backing_file);
    } else {
        path_combine(dest, sz, bs->filename, bs->backing_file);
    }
}
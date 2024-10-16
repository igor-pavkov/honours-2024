static int qcow2_change_backing_file(BlockDriverState *bs,
    const char *backing_file, const char *backing_fmt)
{
    pstrcpy(bs->backing_file, sizeof(bs->backing_file), backing_file ?: "");
    pstrcpy(bs->backing_format, sizeof(bs->backing_format), backing_fmt ?: "");

    return qcow2_update_header(bs);
}
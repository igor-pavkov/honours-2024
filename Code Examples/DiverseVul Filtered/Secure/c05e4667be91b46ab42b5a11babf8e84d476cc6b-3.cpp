static int find_snapshot_by_id_or_name(BlockDriverState *bs,
                                       const char *id_or_name)
{
    int ret;

    ret = find_snapshot_by_id_and_name(bs, id_or_name, NULL);
    if (ret >= 0) {
        return ret;
    }
    return find_snapshot_by_id_and_name(bs, NULL, id_or_name);
}
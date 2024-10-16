static int qcow2_refresh_limits(BlockDriverState *bs)
{
    BDRVQcowState *s = bs->opaque;

    bs->bl.write_zeroes_alignment = s->cluster_sectors;

    return 0;
}
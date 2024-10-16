static int vhdx_check(BlockDriverState *bs, BdrvCheckResult *result,
                       BdrvCheckMode fix)
{
    BDRVVHDXState *s = bs->opaque;

    if (s->log_replayed_on_open) {
        result->corruptions_fixed++;
    }
    return 0;
}
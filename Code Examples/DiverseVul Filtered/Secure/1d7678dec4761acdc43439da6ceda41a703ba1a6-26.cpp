static int vhdx_region_check(BDRVVHDXState *s, uint64_t start, uint64_t length)
{
    int ret = 0;
    uint64_t end;
    VHDXRegionEntry *r;

    end = start + length;
    QLIST_FOREACH(r, &s->regions, entries) {
        if (!((start >= r->end) || (end <= r->start))) {
            ret = -EINVAL;
            goto exit;
        }
    }

exit:
    return ret;
}
static void vhdx_region_register(BDRVVHDXState *s,
                                 uint64_t start, uint64_t length)
{
    VHDXRegionEntry *r;

    r = g_malloc0(sizeof(*r));

    r->start = start;
    r->end = start + length;

    QLIST_INSERT_HEAD(&s->regions, r, entries);
}
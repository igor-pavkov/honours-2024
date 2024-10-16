static void vhdx_region_unregister_all(BDRVVHDXState *s)
{
    VHDXRegionEntry *r, *r_next;

    QLIST_FOREACH_SAFE(r, &s->regions, entries, r_next) {
        QLIST_REMOVE(r, entries);
        g_free(r);
    }
}
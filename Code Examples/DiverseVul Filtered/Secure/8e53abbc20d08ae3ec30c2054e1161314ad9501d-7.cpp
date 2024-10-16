static void bochs_close(BlockDriverState *bs)
{
    BDRVBochsState *s = bs->opaque;
    g_free(s->catalog_bitmap);
}
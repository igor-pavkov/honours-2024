void bdrv_release_dirty_bitmap(BlockDriverState *bs, BdrvDirtyBitmap *bitmap)
{
    BdrvDirtyBitmap *bm, *next;
    QLIST_FOREACH_SAFE(bm, &bs->dirty_bitmaps, list, next) {
        if (bm == bitmap) {
            QLIST_REMOVE(bitmap, list);
            hbitmap_free(bitmap->bitmap);
            g_free(bitmap);
            return;
        }
    }
}
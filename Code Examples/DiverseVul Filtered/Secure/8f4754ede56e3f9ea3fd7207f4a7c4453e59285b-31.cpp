void bdrv_dirty_iter_init(BlockDriverState *bs,
                          BdrvDirtyBitmap *bitmap, HBitmapIter *hbi)
{
    hbitmap_iter_init(hbi, bitmap->bitmap, 0);
}
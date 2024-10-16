int64_t bdrv_get_dirty_count(BlockDriverState *bs, BdrvDirtyBitmap *bitmap)
{
    return hbitmap_count(bitmap->bitmap);
}
static void fill_composite_hash(CompositeHashKey *hk, CombinedBitmapInfo *info)
{
    hk->filter = info->filter;
    hk->bitmap_count = info->bitmap_count;
    hk->bitmaps = info->bitmaps;
}
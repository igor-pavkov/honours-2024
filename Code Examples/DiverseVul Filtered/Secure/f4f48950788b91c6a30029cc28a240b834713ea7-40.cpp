static inline size_t bitmap_size(Bitmap *bm)
{
    return bm ? sizeof(Bitmap) + bm->stride * bm->h : 0;
}
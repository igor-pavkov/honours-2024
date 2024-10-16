static ASS_Image *my_draw_bitmap(unsigned char *bitmap, int bitmap_w,
                                 int bitmap_h, int stride, int dst_x,
                                 int dst_y, uint32_t color,
                                 CompositeHashValue *source)
{
    ASS_ImagePriv *img = malloc(sizeof(ASS_ImagePriv));
    if (!img) {
        if (!source)
            ass_aligned_free(bitmap);
        return NULL;
    }

    img->result.w = bitmap_w;
    img->result.h = bitmap_h;
    img->result.stride = stride;
    img->result.bitmap = bitmap;
    img->result.color = color;
    img->result.dst_x = dst_x;
    img->result.dst_y = dst_y;

    img->source = source;
    ass_cache_inc_ref(source);
    img->ref_count = 0;

    return &img->result;
}
static inline int get_lowest_part_list_y(H264Context *h, Picture *pic, int n,
                                         int height, int y_offset, int list)
{
    int raw_my             = h->mv_cache[list][scan8[n]][1];
    int filter_height_down = (raw_my & 3) ? 3 : 0;
    int full_my            = (raw_my >> 2) + y_offset;
    int bottom             = full_my + filter_height_down + height;

    av_assert2(height >= 0);

    return FFMAX(0, bottom);
}
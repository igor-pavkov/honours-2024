static inline void rectangle_combine(Rectangle *rect, const Bitmap *bm, int x, int y)
{
    rect->x_min = FFMIN(rect->x_min, x + bm->left);
    rect->y_min = FFMIN(rect->y_min, y + bm->top);
    rect->x_max = FFMAX(rect->x_max, x + bm->left + bm->w);
    rect->y_max = FFMAX(rect->y_max, y + bm->top + bm->h);
}
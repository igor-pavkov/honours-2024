static inline void rectangle_reset(Rectangle *rect)
{
    rect->x_min = rect->y_min = INT_MAX;
    rect->x_max = rect->y_max = INT_MIN;
}
void tsc210x_set_transform(uWireSlave *chip,
                MouseTransformInfo *info)
{
    TSC210xState *s = (TSC210xState *) chip->opaque;
#if 0
    int64_t ltr[8];

    ltr[0] = (int64_t) info->a[1] * info->y;
    ltr[1] = (int64_t) info->a[4] * info->x;
    ltr[2] = (int64_t) info->a[1] * info->a[3] -
            (int64_t) info->a[4] * info->a[0];
    ltr[3] = (int64_t) info->a[2] * info->a[4] -
            (int64_t) info->a[5] * info->a[1];
    ltr[4] = (int64_t) info->a[0] * info->y;
    ltr[5] = (int64_t) info->a[3] * info->x;
    ltr[6] = (int64_t) info->a[4] * info->a[0] -
            (int64_t) info->a[1] * info->a[3];
    ltr[7] = (int64_t) info->a[2] * info->a[3] -
            (int64_t) info->a[5] * info->a[0];

    /* Avoid integer overflow */
    s->tr[0] = ltr[0] >> 11;
    s->tr[1] = ltr[1] >> 11;
    s->tr[2] = muldiv64(ltr[2], 1, info->a[6]);
    s->tr[3] = muldiv64(ltr[3], 1 << 4, ltr[2]);
    s->tr[4] = ltr[4] >> 11;
    s->tr[5] = ltr[5] >> 11;
    s->tr[6] = muldiv64(ltr[6], 1, info->a[6]);
    s->tr[7] = muldiv64(ltr[7], 1 << 4, ltr[6]);
#else

    /* This version assumes touchscreen X & Y axis are parallel or
     * perpendicular to LCD's  X & Y axis in some way.  */
    if (abs(info->a[0]) > abs(info->a[1])) {
        s->tr[0] = 0;
        s->tr[1] = -info->a[6] * info->x;
        s->tr[2] = info->a[0];
        s->tr[3] = -info->a[2] / info->a[0];
        s->tr[4] = info->a[6] * info->y;
        s->tr[5] = 0;
        s->tr[6] = info->a[4];
        s->tr[7] = -info->a[5] / info->a[4];
    } else {
        s->tr[0] = info->a[6] * info->y;
        s->tr[1] = 0;
        s->tr[2] = info->a[1];
        s->tr[3] = -info->a[2] / info->a[1];
        s->tr[4] = 0;
        s->tr[5] = -info->a[6] * info->x;
        s->tr[6] = info->a[3];
        s->tr[7] = -info->a[5] / info->a[3];
    }

    s->tr[0] >>= 11;
    s->tr[1] >>= 11;
    s->tr[3] <<= 4;
    s->tr[4] >>= 11;
    s->tr[5] >>= 11;
    s->tr[7] <<= 4;
#endif
}
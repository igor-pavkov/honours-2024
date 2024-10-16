static void copy_picture_range(Picture **to, Picture **from, int count,
                               H264Context *new_base,
                               H264Context *old_base)
{
    int i;

    for (i = 0; i < count; i++) {
        assert((IN_RANGE(from[i], old_base, sizeof(*old_base)) ||
                IN_RANGE(from[i], old_base->DPB,
                         sizeof(Picture) * MAX_PICTURE_COUNT) ||
                !from[i]));
        to[i] = REBASE_PICTURE(from[i], new_base, old_base);
    }
}
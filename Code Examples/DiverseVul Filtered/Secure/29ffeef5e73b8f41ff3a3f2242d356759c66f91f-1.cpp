static int find_unused_picture(H264Context *h)
{
    int i;

    for (i = 0; i < MAX_PICTURE_COUNT; i++) {
        if (pic_is_unused(h, &h->DPB[i]))
            break;
    }
    if (i == MAX_PICTURE_COUNT)
        return AVERROR_INVALIDDATA;

    if (h->DPB[i].needs_realloc) {
        h->DPB[i].needs_realloc = 0;
        unref_picture(h, &h->DPB[i]);
    }

    return i;
}
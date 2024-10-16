static int piz_uncompress(EXRContext *s, const uint8_t *src, int ssize,
                          int dsize, EXRThreadData *td)
{
    GetByteContext gb;
    uint16_t maxval, min_non_zero, max_non_zero;
    uint16_t *ptr;
    uint16_t *tmp = (uint16_t *)td->tmp;
    uint16_t *out;
    uint16_t *in;
    int ret, i, j;
    int pixel_half_size;/* 1 for half, 2 for float and uint32 */
    EXRChannel *channel;
    int tmp_offset;

    if (!td->bitmap)
        td->bitmap = av_malloc(BITMAP_SIZE);
    if (!td->lut)
        td->lut = av_malloc(1 << 17);
    if (!td->bitmap || !td->lut) {
        av_freep(&td->bitmap);
        av_freep(&td->lut);
        return AVERROR(ENOMEM);
    }

    bytestream2_init(&gb, src, ssize);
    min_non_zero = bytestream2_get_le16(&gb);
    max_non_zero = bytestream2_get_le16(&gb);

    if (max_non_zero >= BITMAP_SIZE)
        return AVERROR_INVALIDDATA;

    memset(td->bitmap, 0, FFMIN(min_non_zero, BITMAP_SIZE));
    if (min_non_zero <= max_non_zero)
        bytestream2_get_buffer(&gb, td->bitmap + min_non_zero,
                               max_non_zero - min_non_zero + 1);
    memset(td->bitmap + max_non_zero + 1, 0, BITMAP_SIZE - max_non_zero - 1);

    maxval = reverse_lut(td->bitmap, td->lut);

    ret = huf_uncompress(&gb, tmp, dsize / sizeof(uint16_t));
    if (ret)
        return ret;

    ptr = tmp;
    for (i = 0; i < s->nb_channels; i++) {
        channel = &s->channels[i];

        if (channel->pixel_type == EXR_HALF)
            pixel_half_size = 1;
        else
            pixel_half_size = 2;

        for (j = 0; j < pixel_half_size; j++)
            wav_decode(ptr + j, td->xsize, pixel_half_size, td->ysize,
                       td->xsize * pixel_half_size, maxval);
        ptr += td->xsize * td->ysize * pixel_half_size;
    }

    apply_lut(td->lut, tmp, dsize / sizeof(uint16_t));

    out = (uint16_t *)td->uncompressed_data;
    for (i = 0; i < td->ysize; i++) {
        tmp_offset = 0;
        for (j = 0; j < s->nb_channels; j++) {
            channel = &s->channels[j];
            if (channel->pixel_type == EXR_HALF)
                pixel_half_size = 1;
            else
                pixel_half_size = 2;

            in = tmp + tmp_offset * td->xsize * td->ysize + i * td->xsize * pixel_half_size;
            tmp_offset += pixel_half_size;

#if HAVE_BIGENDIAN
            s->bbdsp.bswap16_buf(out, in, td->xsize * pixel_half_size);
#else
            memcpy(out, in, td->xsize * 2 * pixel_half_size);
#endif
            out += td->xsize * pixel_half_size;
        }
    }

    return 0;
}
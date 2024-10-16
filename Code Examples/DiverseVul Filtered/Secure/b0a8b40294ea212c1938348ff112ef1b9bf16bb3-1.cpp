static int zip_uncompress(EXRContext *s, const uint8_t *src, int compressed_size,
                          int uncompressed_size, EXRThreadData *td)
{
    unsigned long dest_len = uncompressed_size;

    if (uncompress(td->tmp, &dest_len, src, compressed_size) != Z_OK ||
        dest_len != uncompressed_size)
        return AVERROR_INVALIDDATA;

    av_assert1(uncompressed_size % 2 == 0);

    s->dsp.predictor(td->tmp, uncompressed_size);
    s->dsp.reorder_pixels(td->uncompressed_data, td->tmp, uncompressed_size);

    return 0;
}
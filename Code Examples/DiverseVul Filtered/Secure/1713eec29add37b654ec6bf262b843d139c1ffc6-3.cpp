static void output_buffer(int16_t **samples, int nchan, int blocksize,
                          int32_t **buffer)
{
    int i, ch;
    for (ch = 0; ch < nchan; ch++) {
        int32_t *in  = buffer[ch];
        int16_t *out = samples[ch];
        for (i = 0; i < blocksize; i++)
            out[i] = av_clip_int16(in[i]);
    }
}
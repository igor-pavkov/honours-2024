static void imdct_and_windowing_eld(AACContext *ac, SingleChannelElement *sce)
{
    float *in    = sce->coeffs;
    float *out   = sce->ret;
    float *saved = sce->saved;
    const float *const window = ff_aac_eld_window;
    float *buf  = ac->buf_mdct;
    int i;
    const int n  = 512;
    const int n2 = n >> 1;
    const int n4 = n >> 2;

    // Inverse transform, mapped to the conventional IMDCT by
    // Chivukula, R.K.; Reznik, Y.A.; Devarajan, V.,
    // "Efficient algorithms for MPEG-4 AAC-ELD, AAC-LD and AAC-LC filterbanks,"
    // International Conference on Audio, Language and Image Processing, ICALIP 2008.
    // URL: http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4590245&isnumber=4589950
    for (i = 0; i < n2; i+=2) {
        float temp;
        temp =  in[i    ]; in[i    ] = -in[n - 1 - i]; in[n - 1 - i] = temp;
        temp = -in[i + 1]; in[i + 1] =  in[n - 2 - i]; in[n - 2 - i] = temp;
    }
    ac->mdct.imdct_half(&ac->mdct_ld, buf, in);
    for (i = 0; i < n; i+=2) {
        buf[i] = -buf[i];
    }
    // Like with the regular IMDCT at this point we still have the middle half
    // of a transform but with even symmetry on the left and odd symmetry on
    // the right

    // window overlapping
    // The spec says to use samples [0..511] but the reference decoder uses
    // samples [128..639].
    for (i = n4; i < n2; i ++) {
        out[i - n4] =    buf[n2 - 1 - i]       * window[i       - n4] +
                       saved[      i + n2]     * window[i +   n - n4] +
                      -saved[  n + n2 - 1 - i] * window[i + 2*n - n4] +
                      -saved[2*n + n2 + i]     * window[i + 3*n - n4];
    }
    for (i = 0; i < n2; i ++) {
        out[n4 + i] =    buf[i]               * window[i + n2       - n4] +
                      -saved[      n - 1 - i] * window[i + n2 +   n - n4] +
                      -saved[  n + i]         * window[i + n2 + 2*n - n4] +
                       saved[2*n + n - 1 - i] * window[i + n2 + 3*n - n4];
    }
    for (i = 0; i < n4; i ++) {
        out[n2 + n4 + i] =    buf[      i + n2]     * window[i +   n - n4] +
                           -saved[      n2 - 1 - i] * window[i + 2*n - n4] +
                           -saved[  n + n2 + i]     * window[i + 3*n - n4];
    }

    // buffer update
    memmove(saved + n, saved, 2 * n * sizeof(float));
    memcpy( saved,       buf,     n * sizeof(float));
}
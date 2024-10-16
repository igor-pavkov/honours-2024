static int huf_build_dec_table(const uint64_t *hcode, int im,
                               int iM, HufDec *hdecod)
{
    for (; im <= iM; im++) {
        uint64_t c = hcode[im] >> 6;
        int i, l = hcode[im] & 63;

        if (c >> l)
            return AVERROR_INVALIDDATA;

        if (l > HUF_DECBITS) {
            HufDec *pl = hdecod + (c >> (l - HUF_DECBITS));
            if (pl->len)
                return AVERROR_INVALIDDATA;

            pl->lit++;

            pl->p = av_realloc(pl->p, pl->lit * sizeof(int));
            if (!pl->p)
                return AVERROR(ENOMEM);

            pl->p[pl->lit - 1] = im;
        } else if (l) {
            HufDec *pl = hdecod + (c << (HUF_DECBITS - l));

            for (i = 1 << (HUF_DECBITS - l); i > 0; i--, pl++) {
                if (pl->len || pl->p)
                    return AVERROR_INVALIDDATA;
                pl->len = l;
                pl->lit = im;
            }
        }
    }

    return 0;
}
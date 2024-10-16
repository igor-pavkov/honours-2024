static void huf_canonical_code_table(uint64_t *hcode)
{
    uint64_t c, n[59] = { 0 };
    int i;

    for (i = 0; i < HUF_ENCSIZE; ++i)
        n[hcode[i]] += 1;

    c = 0;
    for (i = 58; i > 0; --i) {
        uint64_t nc = ((c + n[i]) >> 1);
        n[i] = c;
        c    = nc;
    }

    for (i = 0; i < HUF_ENCSIZE; ++i) {
        int l = hcode[i];

        if (l > 0)
            hcode[i] = l | (n[l]++ << 6);
    }
}
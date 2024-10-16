int main(int argc, char *argv[]) {
    FILE *in = fopen(argv[1], "rb");
    uint8_t *orig = av_malloc(MAXSZ + 16);
    uint8_t *comp = av_malloc(2*MAXSZ + 16);
    uint8_t *decomp = av_malloc(MAXSZ + 16);
    size_t s = fread(orig, 1, MAXSZ, in);
    lzo_uint clen = 0;
    long tmp[LZO1X_MEM_COMPRESS];
    int inlen, outlen;
    int i;
    av_log_set_level(AV_LOG_DEBUG);
    lzo1x_999_compress(orig, s, comp, &clen, tmp);
    for (i = 0; i < 300; i++) {
START_TIMER
        inlen = clen; outlen = MAXSZ;
#if BENCHMARK_LIBLZO_SAFE
        if (lzo1x_decompress_safe(comp, inlen, decomp, &outlen, NULL))
#elif BENCHMARK_LIBLZO_UNSAFE
        if (lzo1x_decompress(comp, inlen, decomp, &outlen, NULL))
#else
        if (av_lzo1x_decode(decomp, &outlen, comp, &inlen))
#endif
            av_log(NULL, AV_LOG_ERROR, "decompression error\n");
STOP_TIMER("lzod")
    }
    if (memcmp(orig, decomp, s))
        av_log(NULL, AV_LOG_ERROR, "decompression incorrect\n");
    else
        av_log(NULL, AV_LOG_ERROR, "decompression OK\n");
    return 0;
}
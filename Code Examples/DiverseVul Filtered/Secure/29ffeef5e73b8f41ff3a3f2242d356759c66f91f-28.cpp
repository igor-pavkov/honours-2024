static av_always_inline void prefetch_motion(H264Context *h, int list,
                                             int pixel_shift, int chroma_idc)
{
    /* fetch pixels for estimated mv 4 macroblocks ahead
     * optimized for 64byte cache lines */
    const int refn = h->ref_cache[list][scan8[0]];
    if (refn >= 0) {
        const int mx  = (h->mv_cache[list][scan8[0]][0] >> 2) + 16 * h->mb_x + 8;
        const int my  = (h->mv_cache[list][scan8[0]][1] >> 2) + 16 * h->mb_y;
        uint8_t **src = h->ref_list[list][refn].f.data;
        int off       = (mx << pixel_shift) +
                        (my + (h->mb_x & 3) * 4) * h->mb_linesize +
                        (64 << pixel_shift);
        h->vdsp.prefetch(src[0] + off, h->linesize, 4);
        if (chroma_idc == 3 /* yuv444 */) {
            h->vdsp.prefetch(src[1] + off, h->linesize, 4);
            h->vdsp.prefetch(src[2] + off, h->linesize, 4);
        } else {
            off= (((mx>>1)+64)<<pixel_shift) + ((my>>1) + (h->mb_x&7))*h->uvlinesize;
            h->vdsp.prefetch(src[1] + off, src[2] - src[1], 2);
        }
    }
}
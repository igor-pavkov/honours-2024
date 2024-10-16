static inline int als_weighting(GetBitContext *gb, int k, int off)
{
    int idx = av_clip(decode_rice(gb, k) + off,
                      0, FF_ARRAY_ELEMS(mcc_weightings) - 1);
    return mcc_weightings[idx];
}
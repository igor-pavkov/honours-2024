static void decode_ltp(LongTermPrediction *ltp,
                       GetBitContext *gb, uint8_t max_sfb)
{
    int sfb;

    ltp->lag  = get_bits(gb, 11);
    ltp->coef = ltp_coef[get_bits(gb, 3)];
    for (sfb = 0; sfb < FFMIN(max_sfb, MAX_LTP_LONG_SFB); sfb++)
        ltp->used[sfb] = get_bits1(gb);
}
static int decode_new_pred(Mpeg4DecContext *ctx, GetBitContext *gb) {
    int len = FFMIN(ctx->time_increment_bits + 3, 15);

    get_bits(gb, len);
    if (get_bits1(gb))
        get_bits(gb, len);
    check_marker(gb, "after new_pred");

    return 0;
}
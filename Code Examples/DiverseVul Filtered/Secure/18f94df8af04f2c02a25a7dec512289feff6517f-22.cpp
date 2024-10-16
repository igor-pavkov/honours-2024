static int read_decode_block(ALSDecContext *ctx, ALSBlockData *bd)
{
    int ret;

    if ((ret = read_block(ctx, bd)) < 0)
        return ret;

    return decode_block(ctx, bd);
}
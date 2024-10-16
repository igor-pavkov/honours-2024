static int read_block(ALSDecContext *ctx, ALSBlockData *bd)
{
    int ret;
    GetBitContext *gb        = &ctx->gb;

    *bd->shift_lsbs = 0;
    // read block type flag and read the samples accordingly
    if (get_bits1(gb)) {
        ret = read_var_block_data(ctx, bd);
    } else {
        ret = read_const_block_data(ctx, bd);
    }

    return ret;
}
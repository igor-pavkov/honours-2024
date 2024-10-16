static int decode_block(ALSDecContext *ctx, ALSBlockData *bd)
{
    unsigned int smp;
    int ret = 0;

    // read block type flag and read the samples accordingly
    if (*bd->const_block)
        decode_const_block_data(ctx, bd);
    else
        ret = decode_var_block_data(ctx, bd); // always return 0

    if (ret < 0)
        return ret;

    // TODO: read RLSLMS extension data

    if (*bd->shift_lsbs)
        for (smp = 0; smp < bd->block_length; smp++)
            bd->raw_samples[smp] <<= *bd->shift_lsbs;

    return 0;
}
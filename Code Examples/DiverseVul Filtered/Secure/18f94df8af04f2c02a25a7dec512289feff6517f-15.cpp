static void parse_bs_info(const uint32_t bs_info, unsigned int n,
                          unsigned int div, unsigned int **div_blocks,
                          unsigned int *num_blocks)
{
    if (n < 31 && ((bs_info << n) & 0x40000000)) {
        // if the level is valid and the investigated bit n is set
        // then recursively check both children at bits (2n+1) and (2n+2)
        n   *= 2;
        div += 1;
        parse_bs_info(bs_info, n + 1, div, div_blocks, num_blocks);
        parse_bs_info(bs_info, n + 2, div, div_blocks, num_blocks);
    } else {
        // else the bit is not set or the last level has been reached
        // (bit implicitly not set)
        **div_blocks = div;
        (*div_blocks)++;
        (*num_blocks)++;
    }
}
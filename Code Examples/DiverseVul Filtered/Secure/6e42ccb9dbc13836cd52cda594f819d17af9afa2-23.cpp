static int decode_drc_channel_exclusions(DynamicRangeControl *che_drc,
                                         GetBitContext *gb)
{
    int i;
    int num_excl_chan = 0;

    do {
        for (i = 0; i < 7; i++)
            che_drc->exclude_mask[num_excl_chan++] = get_bits1(gb);
    } while (num_excl_chan < MAX_CHANNELS - 7 && get_bits1(gb));

    return num_excl_chan / 7;
}
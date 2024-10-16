static int ok_inflater_decode_distance(ok_inflater *inflater,
                                       const ok_inflater_huffman_tree *tree) {
    if (!ok_inflater_load_bits(inflater, tree->bits)) {
        return -1;
    }
    uint32_t p = ok_inflater_peek_bits(inflater, tree->bits);
    uint16_t tree_value = tree->lookup_table[p];
    int value = tree_value & VALUE_BIT_MASK;
    unsigned int value_bits = tree_value >> VALUE_BITS;
    if (value < 4) {
        ok_inflater_read_bits(inflater, value_bits);
        return value + 1;
    } else if (value >= OK_INFLATER_DISTANCE_TABLE_LENGTH) {
        ok_inflater_error(inflater, "Invalid distance");
        return -1;
    } else {
        unsigned int extra_bits = (unsigned int)((value >> 1) - 1);
        if (!ok_inflater_can_read_bits(inflater, value_bits + extra_bits)) {
            return -1;
        }
        // Make sure to load no more than 32 bits at once
        ok_inflater_read_bits(inflater, value_bits);
        ok_inflater_load_bits(inflater, extra_bits);
        int d = (int)ok_inflater_read_bits(inflater, extra_bits);
        return OK_INFLATER_DISTANCE_TABLE[value] + d;
    }
}
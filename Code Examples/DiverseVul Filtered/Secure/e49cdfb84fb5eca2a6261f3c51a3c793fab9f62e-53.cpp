static int ok_inflater_decode_literal(ok_inflater *inflater, const uint16_t *tree_lookup_table,
                                      unsigned int tree_bits) {
    if (!ok_inflater_load_bits(inflater, tree_bits)) {
        return -1;
    }
    uint32_t p = ok_inflater_peek_bits(inflater, tree_bits);
    uint16_t value = tree_lookup_table[p];
    ok_inflater_read_bits(inflater, value >> VALUE_BITS);
    return value & VALUE_BIT_MASK;
}
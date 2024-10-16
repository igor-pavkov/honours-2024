static bool ok_inflater_dynamic_block_code_lengths(ok_inflater *inflater) {
    while (inflater->state_count > 0) {
        if (!ok_inflater_load_bits(inflater, 3)) {
            return false;
        }
        int index = inflater->num_code_length_codes - inflater->state_count;
        inflater->tree_codes[OK_INFLATER_BIT_LENGTH_TABLE[index]] =
            (uint8_t)ok_inflater_read_bits(inflater, 3);
        inflater->state_count--;
    }
    ok_inflater_make_huffman_tree_from_array(inflater->code_length_huffman,
                                             inflater->tree_codes,
                                             OK_INFLATER_BIT_LENGTH_TABLE_LENGTH);

    inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_LITERAL_TREE;
    inflater->huffman_code = -1;
    inflater->state_count = 0;
    return true;
}
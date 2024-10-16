static bool ok_inflater_compressed_block(ok_inflater *inflater) {
    const bool is_fixed = inflater->state == OK_INFLATER_STATE_READING_FIXED_COMPRESSED_BLOCK;
    const ok_inflater_huffman_tree *literal_tree =
        (is_fixed ? inflater->fixed_literal_huffman : inflater->literal_huffman);
    const ok_inflater_huffman_tree *distance_tree =
        (is_fixed ? inflater->fixed_distance_huffman : inflater->distance_huffman);

    // decode literal/length value from input stream

    size_t max_write = ok_inflater_can_write_total(inflater);
    const uint16_t *tree_lookup_table = literal_tree->lookup_table;
    const unsigned int tree_bits = literal_tree->bits;
    while (max_write > 0) {
        int value = ok_inflater_decode_literal(inflater, tree_lookup_table, tree_bits);
        if (value < 0) {
            // Needs input
            return false;
        } else if (value < 256) {
            ok_inflater_write_byte(inflater, (uint8_t)value);
            max_write--;
        } else if (value == 256) {
            inflater->state = OK_INFLATER_STATE_READY_FOR_NEXT_BLOCK;
            return true;
        } else if (value < 286) {
            inflater->huffman_code = value - 257;
            inflater->state_count = -1;
            inflater->state_distance = -1;
            if (ok_inflater_distance_with_tree(inflater, distance_tree)) {
                max_write = ok_inflater_can_write_total(inflater);
            } else {
                if (is_fixed) {
                    inflater->state = OK_INFLATER_STATE_READING_FIXED_DISTANCE;
                } else {
                    inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_DISTANCE;
                }
                return false;
            }
        } else {
            ok_inflater_error(inflater, "Invalid inflater literal");
            return false;
        }
    }
    // Output buffer full
    return false;
}
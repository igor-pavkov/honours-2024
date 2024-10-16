static bool ok_inflater_inflate_huffman_tree(ok_inflater *inflater, ok_inflater_huffman_tree *tree,
                                             ok_inflater_huffman_tree *code_length_huffman,
                                             int num_codes) {
    if (num_codes < 0 || num_codes >= MAX_NUM_CODES) {
        ok_inflater_error(inflater, "Invalid num_codes");
        return false;
    }
    const uint16_t *tree_lookup_table = code_length_huffman->lookup_table;
    const unsigned int tree_bits = code_length_huffman->bits;
    // 0 - 15: Represent code lengths of 0 - 15
    //     16: Copy the previous code length 3 - 6 times.
    //         (2 bits of length)
    //     17: Repeat a code length of 0 for 3 - 10 times.
    //         (3 bits of length)
    //     18: Repeat a code length of 0 for 11 - 138 times
    //         (7 bits of length)
    while (inflater->state_count < num_codes) {
        if (inflater->huffman_code < 0) {
            inflater->huffman_code = ok_inflater_decode_literal(inflater, tree_lookup_table,
                                                                tree_bits);
            if (inflater->huffman_code < 0) {
                return false;
            }
        }
        if (inflater->huffman_code <= 15) {
            inflater->tree_codes[inflater->state_count++] = (uint8_t)inflater->huffman_code;
        } else {
            int value = 0;
            int len;
            unsigned int len_bits;
            switch (inflater->huffman_code) {
                case 16:
                    len = 3;
                    len_bits = 2;
                    if (inflater->state_count == 0) {
                        ok_inflater_error(inflater, "Invalid previous code");
                        return false;
                    }
                    value = inflater->tree_codes[inflater->state_count - 1];
                    break;
                case 17:
                    len = 3;
                    len_bits = 3;
                    break;
                case 18:
                    len = 11;
                    len_bits = 7;
                    break;
                default:
                    ok_inflater_error(inflater, "Invalid huffman code");
                    return false;
            }
            if (!ok_inflater_load_bits(inflater, len_bits)) {
                return false;
            }
            len += ok_inflater_read_bits(inflater, len_bits);
            if (len > num_codes - inflater->state_count) {
                ok_inflater_error(inflater, "Invalid length");
                return false;
            }
            memset(inflater->tree_codes + inflater->state_count, value, (size_t)len);
            inflater->state_count += len;
        }
        inflater->huffman_code = -1;
    }
    ok_inflater_make_huffman_tree_from_array(tree, inflater->tree_codes, num_codes);
    return true;
}
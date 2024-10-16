static bool ok_inflater_init_fixed_huffman(ok_inflater *inflater) {
    if (!inflater->fixed_literal_huffman) {
        ok_inflater_huffman_tree *tree = ok_alloc(inflater, sizeof(ok_inflater_huffman_tree));
        if (tree) {
            uint8_t code_length[288];
            int i;
            for (i = 0; i < 144; i++) {
                code_length[i] = 8;
            }
            for (i = 144; i < 256; i++) {
                code_length[i] = 9;
            }
            for (i = 256; i < 280; i++) {
                code_length[i] = 7;
            }
            for (i = 280; i < 288; i++) {
                code_length[i] = 8;
            }
            ok_inflater_make_huffman_tree_from_array(tree, code_length,
                                                     sizeof(code_length) / sizeof(code_length[0]));
            inflater->fixed_literal_huffman = tree;
        }
    }
    if (!inflater->fixed_distance_huffman) {
        ok_inflater_huffman_tree *tree = ok_alloc(inflater, sizeof(ok_inflater_huffman_tree));
        if (tree) {
            uint8_t distance_code_length[32];
            for (int i = 0; i < 32; i++) {
                distance_code_length[i] = 5;
            }
            ok_inflater_make_huffman_tree_from_array(tree, distance_code_length, 32);
            inflater->fixed_distance_huffman = tree;
        }
    }
    return inflater->fixed_literal_huffman && inflater->fixed_distance_huffman;
}
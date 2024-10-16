static bool ok_inflater_literal_tree(ok_inflater *inflater) {
    bool done = ok_inflater_inflate_huffman_tree(inflater, inflater->literal_huffman,
                                                 inflater->code_length_huffman,
                                                 inflater->num_literal_codes);
    if (done) {
        inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_DISTANCE_TREE;
        inflater->huffman_code = -1;
        inflater->state_count = 0;
        return true;
    } else {
        return false;
    }
}
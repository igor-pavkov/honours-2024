static bool ok_inflater_distance_tree(ok_inflater *inflater) {
    bool done = ok_inflater_inflate_huffman_tree(inflater, inflater->distance_huffman,
                                                 inflater->code_length_huffman,
                                                 inflater->num_distance_codes);
    if (done) {
        inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_COMPRESSED_BLOCK;
        inflater->huffman_code = -1;
        return true;
    } else {
        return false;
    }
}
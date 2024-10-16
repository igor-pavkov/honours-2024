static bool ok_inflater_distance(ok_inflater *inflater) {
    bool is_fixed = inflater->state == OK_INFLATER_STATE_READING_FIXED_DISTANCE;
    const ok_inflater_huffman_tree *distance_tree =
        (is_fixed ? inflater->fixed_distance_huffman : inflater->distance_huffman);
    if (ok_inflater_distance_with_tree(inflater, distance_tree)) {
        if (is_fixed) {
            inflater->state = OK_INFLATER_STATE_READING_FIXED_COMPRESSED_BLOCK;
        } else {
            inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_COMPRESSED_BLOCK;
        }
        return true;
    } else {
        return false;
    }
}
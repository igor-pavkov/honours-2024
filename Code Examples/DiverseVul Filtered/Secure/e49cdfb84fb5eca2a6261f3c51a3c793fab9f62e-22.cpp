static bool ok_inflater_distance_with_tree(ok_inflater *inflater,
                                           const ok_inflater_huffman_tree *tree) {
    if (inflater->state_count < 0) {
        inflater->state_count = ok_inflater_decode_length(inflater, inflater->huffman_code);
        if (inflater->state_count < 0) {
            // Needs input
            return false;
        }
        inflater->huffman_code = -1;
    }
    if (inflater->state_distance < 0) {
        inflater->state_distance = ok_inflater_decode_distance(inflater, tree);
        if (inflater->state_distance < 0) {
            // Needs input
            return false;
        }
    }

    // Copy len bytes from offset to buffer_end_pos
    int buffer_offset = (inflater->buffer_end_pos - inflater->state_distance) & BUFFER_SIZE_MASK;
    if (inflater->state_distance == 1) {
        // Optimization: can use memset
        int n = inflater->state_count;
        int n2 = ok_inflater_write_byte_n(inflater, inflater->buffer[buffer_offset], n);
        inflater->state_count -= n2;
        if (n2 != n) {
            // Full buffer
            return false;
        }
    } else if (buffer_offset + inflater->state_count < BUFFER_SIZE) {
        // Optimization: the offset won't wrap
        int bytes_copyable = inflater->state_distance;
        while (inflater->state_count > 0) {
            int n = min(inflater->state_count, bytes_copyable);
            int n2 = ok_inflater_write_bytes(inflater, inflater->buffer + buffer_offset, n);
            inflater->state_count -= n2;
            bytes_copyable += n2;
            if (n2 != n) {
                // Full buffer
                return false;
            }
        }
    } else {
        // This could be optimized, but it happens rarely, so it's probably not worth it
        while (inflater->state_count > 0) {
            int n = min(inflater->state_count, inflater->state_distance);
            n = min(n, (BUFFER_SIZE - buffer_offset));
            int n2 = ok_inflater_write_bytes(inflater, inflater->buffer + buffer_offset, n);
            inflater->state_count -= n2;
            buffer_offset = (buffer_offset + n2) & BUFFER_SIZE_MASK;
            if (n2 != n) {
                // Full buffer
                return false;
            }
        }
    }
    return true;
}
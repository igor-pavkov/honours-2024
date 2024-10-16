static bool ok_inflater_next_block(ok_inflater *inflater) {
    if (inflater->final_block) {
        inflater->state = OK_INFLATER_STATE_DONE;
        ok_inflater_skip_byte_align(inflater);
        return true;
    } else if (!ok_inflater_load_bits(inflater, 3)) {
        return false;
    } else {
        inflater->final_block = ok_inflater_read_bits(inflater, 1);
        uint32_t block_type = ok_inflater_read_bits(inflater, 2);
        switch (block_type) {
            case BLOCK_TYPE_NO_COMPRESSION:
                inflater->state = OK_INFLATER_STATE_READING_STORED_BLOCK_HEADER;
                break;
            case BLOCK_TYPE_DYNAMIC_HUFFMAN:
                inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_BLOCK_HEADER;
                break;
            case BLOCK_TYPE_FIXED_HUFFMAN: {
                if (!ok_inflater_init_fixed_huffman(inflater)) {
                    ok_inflater_error(inflater, "Couldn't initilize fixed huffman trees");
                    return false;
                }
                inflater->state = OK_INFLATER_STATE_READING_FIXED_COMPRESSED_BLOCK;
                inflater->huffman_code = -1;
                break;
            }
            default:
                ok_inflater_error(inflater, "Invalid block type");
                break;
        }
        return true;
    }
}
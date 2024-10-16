static bool ok_inflater_stored_block_header(ok_inflater *inflater) {
    ok_inflater_skip_byte_align(inflater);
    if (!ok_inflater_load_bits(inflater, 32)) {
        return false;
    } else {
        uint32_t len = ok_inflater_read_bits(inflater, 16);
        uint32_t clen = ok_inflater_read_bits(inflater, 16);
        if ((len & 0xffff) != ((~clen) & 0xffff)) {
            ok_inflater_error(inflater, "Invalid stored block");
            return false;
        } else if (len == 0) {
            inflater->state = OK_INFLATER_STATE_READY_FOR_NEXT_BLOCK;
            return true;
        } else {
            inflater->state = OK_INFLATER_STATE_READING_STORED_BLOCK;
            inflater->state_count = (int)len;
            return true;
        }
    }
}
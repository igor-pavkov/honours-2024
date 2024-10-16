static bool ok_inflater_zlib_header(ok_inflater *inflater) {
    if (!ok_inflater_load_bits(inflater, 16)) {
        return false;
    } else {
        uint32_t compression_method = ok_inflater_read_bits(inflater, 4);
        uint32_t compression_info = ok_inflater_read_bits(inflater, 4);
        uint32_t flag_check = ok_inflater_read_bits(inflater, 5);
        uint32_t flag_dict = ok_inflater_read_bits(inflater, 1);
        uint32_t flag_compression_level = ok_inflater_read_bits(inflater, 2);

        uint32_t bits = ((compression_info << 12) | (compression_method << 8) |
                         (flag_compression_level << 6) | (flag_dict << 5) | flag_check);
        if (bits % 31 != 0) {
            ok_inflater_error(inflater, "Invalid zlib header");
            return false;
        }
        if (compression_method != 8) {
            ok_inflater_error(inflater, "Invalid inflater compression method");
            return false;
        }
        if (compression_info > 7) {
            ok_inflater_error(inflater, "Invalid window size");
            return false;
        }
        if (flag_dict) {
            ok_inflater_error(inflater, "Needs external dictionary");
            return false;
        }

        inflater->state = OK_INFLATER_STATE_READY_FOR_NEXT_BLOCK;
        return true;
    }
}
static inline bool ok_inflater_can_read_bits(ok_inflater *inflater, unsigned int num_bits) {
    if (ok_inflater_load_bits(inflater, num_bits)) {
        return true;
    }
    int bytes_needed = (num_bits - inflater->input_buffer_bits + 7) >> 3;
    return bytes_needed <= (inflater->input_end - inflater->input);
}
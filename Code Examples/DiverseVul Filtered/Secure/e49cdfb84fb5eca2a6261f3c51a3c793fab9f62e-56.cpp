static inline bool ok_inflater_load_bits(ok_inflater *inflater, unsigned int num_bits) {
    while (inflater->input_buffer_bits < num_bits) {
        if (inflater->input == inflater->input_end || inflater->input_buffer_bits + 8 > 32) {
            return false;
        }
        uint32_t input = *inflater->input++;
        inflater->input_buffer |= input << inflater->input_buffer_bits;
        inflater->input_buffer_bits += 8;
    }
    return true;
}
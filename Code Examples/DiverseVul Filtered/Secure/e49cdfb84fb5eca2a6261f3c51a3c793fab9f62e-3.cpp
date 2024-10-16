static inline uint32_t ok_inflater_read_bits(ok_inflater *inflater, unsigned int num_bits) {
    uint32_t ans = inflater->input_buffer & ((1 << num_bits) - 1);
    inflater->input_buffer >>= num_bits;
    inflater->input_buffer_bits -= num_bits;
    return ans;
}
static inline uint32_t ok_inflater_peek_bits(ok_inflater *inflater, unsigned int num_bits) {
    return inflater->input_buffer & ((1 << num_bits) - 1);
}
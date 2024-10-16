static inline uint32_t ok_inflater_reverse_bits(uint32_t value, unsigned int num_bits) {
    uint32_t rev_value = value & 1;
    for (unsigned int i = num_bits - 1; i > 0; i--) {
        value >>= 1;
        rev_value <<= 1;
        rev_value |= value & 1;
    }
    return rev_value;
}
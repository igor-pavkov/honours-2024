static inline void ok_inflater_skip_byte_align(ok_inflater *inflater) {
    unsigned int skip_bits = inflater->input_buffer_bits & 7;
    inflater->input_buffer >>= skip_bits;
    inflater->input_buffer_bits -= skip_bits;
}
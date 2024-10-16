static inline void ok_inflater_write_byte(ok_inflater *inflater, const uint8_t b) {
    inflater->buffer[inflater->buffer_end_pos & BUFFER_SIZE_MASK] = b;
    inflater->buffer_end_pos++;
}
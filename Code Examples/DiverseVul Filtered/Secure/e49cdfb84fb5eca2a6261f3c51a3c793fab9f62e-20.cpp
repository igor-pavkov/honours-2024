static inline uint16_t ok_inflater_can_write_total(const ok_inflater *inflater) {
    return inflater->buffer_start_pos - inflater->buffer_end_pos - 1;
}
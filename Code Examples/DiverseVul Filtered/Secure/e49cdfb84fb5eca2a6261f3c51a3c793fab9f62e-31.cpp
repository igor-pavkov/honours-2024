static inline uint16_t ok_inflater_can_flush_total(const ok_inflater *inflater) {
    return inflater->buffer_end_pos - inflater->buffer_start_pos;
}
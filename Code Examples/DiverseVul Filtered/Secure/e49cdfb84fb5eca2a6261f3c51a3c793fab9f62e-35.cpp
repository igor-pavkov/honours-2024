static inline uint16_t ok_inflater_can_flush(const ok_inflater *inflater) {
    if (inflater->buffer_start_pos <= inflater->buffer_end_pos) {
        return inflater->buffer_end_pos - inflater->buffer_start_pos;
    } else {
        return -inflater->buffer_start_pos;
    }
}
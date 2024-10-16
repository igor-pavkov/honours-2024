static inline uint16_t ok_inflater_can_write(const ok_inflater *inflater) {
    if (inflater->buffer_start_pos == 0) {
        return -inflater->buffer_end_pos - 1;
    } else if (inflater->buffer_start_pos > inflater->buffer_end_pos) {
        return inflater->buffer_start_pos - inflater->buffer_end_pos - 1;
    } else {
        return -inflater->buffer_end_pos;
    }
}
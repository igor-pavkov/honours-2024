static inline size_t ok_inflater_flush(ok_inflater *inflater, uint8_t *dst, size_t len) {
    size_t bytes_remaining = len;
    while (bytes_remaining > 0) {
        size_t n = min(bytes_remaining, ok_inflater_can_flush(inflater));
        if (n == 0) {
            return len - bytes_remaining;
        }
        memcpy(dst, inflater->buffer + inflater->buffer_start_pos, n);
        inflater->buffer_start_pos += n;
        bytes_remaining -= n;
        dst += n;
    }
    return len;
}
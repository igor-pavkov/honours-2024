static inline int ok_inflater_write_byte_n(ok_inflater *inflater, const uint8_t b, int len) {
    int bytes_remaining = len;
    while (bytes_remaining > 0) {
        int n = min(bytes_remaining, ok_inflater_can_write(inflater));
        if (n == 0) {
            return len - bytes_remaining;
        }
        memset(inflater->buffer + inflater->buffer_end_pos, b, (size_t)n);
        inflater->buffer_end_pos += n;
        bytes_remaining -= n;
    }
    return len;
}
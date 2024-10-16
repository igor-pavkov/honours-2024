static inline int ok_inflater_write_bytes(ok_inflater *inflater, const uint8_t *src, int len) {
    int bytes_remaining = len;
    while (bytes_remaining > 0) {
        int n = min(bytes_remaining, ok_inflater_can_write(inflater));
        if (n == 0) {
            return len - bytes_remaining;
        }
        memcpy(inflater->buffer + inflater->buffer_end_pos, src, (size_t)n);
        inflater->buffer_end_pos += n;
        bytes_remaining -= n;
        src += n;
    }
    return len;
}
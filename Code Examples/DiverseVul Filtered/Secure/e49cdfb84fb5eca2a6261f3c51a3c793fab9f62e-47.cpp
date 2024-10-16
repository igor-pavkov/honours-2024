size_t ok_inflater_inflate(ok_inflater *inflater, uint8_t *dst, size_t dst_len) {
    if (!inflater || inflater->state == OK_INFLATER_STATE_ERROR ||
        inflater->state == OK_INFLATER_STATE_DONE) {
        return OK_SIZE_MAX;
    }

    // Each state function should return false if input is needed or the buffer is full.
    // Run until one condition occurs:
    // 1. Output buffer can be filled,
    // 2. Internal buffer is full,
    // 3. Needs more input,
    // 4. Done inflating, or
    // 5. An error occured.
    while (ok_inflater_can_flush_total(inflater) < dst_len &&
           (*OK_INFLATER_STATE_FUNCTIONS[inflater->state])(inflater)) {
    }
    return ok_inflater_flush(inflater, dst, dst_len);
}
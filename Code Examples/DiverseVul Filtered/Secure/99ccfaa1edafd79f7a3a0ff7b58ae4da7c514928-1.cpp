static inline int64_t pcnet_get_next_poll_time(PCNetState *s, int64_t current_time)
{
    int64_t next_time = current_time +
                        (65536 - (CSR_SPND(s) ? 0 : CSR_POLL(s))) * 30;

    if (next_time <= current_time) {
        next_time = current_time + 1;
    }
    return next_time;
}
int modbus_set_indication_timeout(modbus_t *ctx, uint32_t to_sec, uint32_t to_usec)
{
    /* Indication timeout can be disabled when both values are zero */
    if (ctx == NULL || to_usec > 999999) {
        errno = EINVAL;
        return -1;
    }

    ctx->indication_timeout.tv_sec = to_sec;
    ctx->indication_timeout.tv_usec = to_usec;
    return 0;
}
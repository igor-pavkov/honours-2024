static void GCC_FMT_ATTR(3, 4) report_unsupported(BlockDriverState *bs,
    Error **errp, const char *fmt, ...)
{
    char msg[64];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    error_set(errp, QERR_UNKNOWN_BLOCK_FORMAT_FEATURE, bs->device_name, "qcow2",
              msg);
}
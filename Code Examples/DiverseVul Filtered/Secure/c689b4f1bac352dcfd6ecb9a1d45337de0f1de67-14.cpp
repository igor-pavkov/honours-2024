void qmp_guest_fstrim(bool has_minimum, int64_t minimum, Error **err)
{
    error_set(err, QERR_UNSUPPORTED);
}
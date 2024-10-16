void qmp_guest_suspend_disk(Error **err)
{
    error_set(err, QERR_UNSUPPORTED);
}
void qmp_guest_suspend_ram(Error **err)
{
    error_set(err, QERR_UNSUPPORTED);
}
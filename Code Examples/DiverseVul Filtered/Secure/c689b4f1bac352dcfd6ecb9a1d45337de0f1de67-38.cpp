int64_t qmp_guest_set_vcpus(GuestLogicalProcessorList *vcpus, Error **errp)
{
    error_set(errp, QERR_UNSUPPORTED);
    return -1;
}
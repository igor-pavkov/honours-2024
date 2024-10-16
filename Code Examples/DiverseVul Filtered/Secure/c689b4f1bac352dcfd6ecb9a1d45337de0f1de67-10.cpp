void qmp_guest_suspend_disk(Error **err)
{
    bios_supports_mode("pm-is-supported", "--hibernate", "disk", err);
    if (error_is_set(err)) {
        return;
    }

    guest_suspend("pm-hibernate", "disk", err);
}
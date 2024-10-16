void qmp_guest_suspend_hybrid(Error **err)
{
    bios_supports_mode("pm-is-supported", "--suspend-hybrid", NULL, err);
    if (error_is_set(err)) {
        return;
    }

    guest_suspend("pm-suspend-hybrid", NULL, err);
}
void qmp_guest_suspend_ram(Error **err)
{
    bios_supports_mode("pm-is-supported", "--suspend", "mem", err);
    if (error_is_set(err)) {
        return;
    }

    guest_suspend("pm-suspend", "mem", err);
}
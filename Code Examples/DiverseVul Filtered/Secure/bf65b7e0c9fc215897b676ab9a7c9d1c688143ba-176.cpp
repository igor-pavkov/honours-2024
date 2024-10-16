int unit_test_start_limit(Unit *u) {
        const char *reason;

        assert(u);

        if (ratelimit_below(&u->start_limit)) {
                u->start_limit_hit = false;
                return 0;
        }

        log_unit_warning(u, "Start request repeated too quickly.");
        u->start_limit_hit = true;

        reason = strjoina("unit ", u->id, " failed");

        emergency_action(u->manager, u->start_limit_action,
                         EMERGENCY_ACTION_IS_WATCHDOG|EMERGENCY_ACTION_WARN,
                         u->reboot_arg, -1, reason);

        return -ECANCELED;
}
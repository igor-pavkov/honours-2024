static int operation_to_signal(KillContext *c, KillOperation k) {
        assert(c);

        switch (k) {

        case KILL_TERMINATE:
        case KILL_TERMINATE_AND_LOG:
                return c->kill_signal;

        case KILL_KILL:
                return c->final_kill_signal;

        case KILL_WATCHDOG:
                return c->watchdog_signal;

        default:
                assert_not_reached("KillOperation unknown");
        }
}
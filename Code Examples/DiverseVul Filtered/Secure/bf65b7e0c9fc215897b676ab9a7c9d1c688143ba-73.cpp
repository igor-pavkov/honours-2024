void unit_log_success(Unit *u) {
        assert(u);

        log_struct(LOG_INFO,
                   "MESSAGE_ID=" SD_MESSAGE_UNIT_SUCCESS_STR,
                   LOG_UNIT_ID(u),
                   LOG_UNIT_INVOCATION_ID(u),
                   LOG_UNIT_MESSAGE(u, "Succeeded."));
}
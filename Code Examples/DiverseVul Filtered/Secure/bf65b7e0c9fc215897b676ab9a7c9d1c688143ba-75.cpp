void unit_log_failure(Unit *u, const char *result) {
        assert(u);
        assert(result);

        log_struct(LOG_WARNING,
                   "MESSAGE_ID=" SD_MESSAGE_UNIT_FAILURE_RESULT_STR,
                   LOG_UNIT_ID(u),
                   LOG_UNIT_INVOCATION_ID(u),
                   LOG_UNIT_MESSAGE(u, "Failed with result '%s'.", result),
                   "UNIT_RESULT=%s", result);
}
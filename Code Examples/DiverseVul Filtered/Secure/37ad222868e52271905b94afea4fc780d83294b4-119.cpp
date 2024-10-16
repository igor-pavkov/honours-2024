static void fill_atomic_prefix(void)
{
    char tmp_atomic_prefix[PATH_MAX];

    snprintf(tmp_atomic_prefix, sizeof tmp_atomic_prefix,
             "%s%lx.%x.%lx.%x",
             ATOMIC_PREFIX_PREFIX,
             (unsigned long) session_start_time,
             (unsigned int) serverport,
             (unsigned long) getpid(),
             zrand());
    if ((atomic_prefix = strdup(tmp_atomic_prefix)) == NULL) {
        die_mem();
    }
}
static int manager_dispatch_signal_fd(sd_event_source *source, int fd, uint32_t revents, void *userdata) {
        Manager *m = userdata;
        ssize_t n;
        struct signalfd_siginfo sfsi;
        bool sigchld = false;
        int r;

        assert(m);
        assert(m->signal_fd == fd);

        if (revents != EPOLLIN) {
                log_warning("Got unexpected events from signal file descriptor.");
                return 0;
        }

        for (;;) {
                n = read(m->signal_fd, &sfsi, sizeof(sfsi));
                if (n != sizeof(sfsi)) {

                        if (n >= 0)
                                return -EIO;

                        if (errno == EINTR || errno == EAGAIN)
                                break;

                        return -errno;
                }

                log_received_signal(sfsi.ssi_signo == SIGCHLD ||
                                    (sfsi.ssi_signo == SIGTERM && MANAGER_IS_USER(m))
                                    ? LOG_DEBUG : LOG_INFO,
                                    &sfsi);

                switch (sfsi.ssi_signo) {

                case SIGCHLD:
                        sigchld = true;
                        break;

                case SIGTERM:
                        if (MANAGER_IS_SYSTEM(m)) {
                                /* This is for compatibility with the
                                 * original sysvinit */
                                m->exit_code = MANAGER_REEXECUTE;
                                break;
                        }

                        /* Fall through */

                case SIGINT:
                        if (MANAGER_IS_SYSTEM(m)) {

                                /* If the user presses C-A-D more than
                                 * 7 times within 2s, we reboot
                                 * immediately. */

                                if (ratelimit_test(&m->ctrl_alt_del_ratelimit))
                                        manager_start_target(m, SPECIAL_CTRL_ALT_DEL_TARGET, JOB_REPLACE_IRREVERSIBLY);
                                else {
                                        log_notice("Ctrl-Alt-Del was pressed more than 7 times within 2s, rebooting immediately.");
                                        status_printf(NULL, true, false, "Ctrl-Alt-Del was pressed more than 7 times within 2s, rebooting immediately.");
                                        m->exit_code = MANAGER_REBOOT;
                                }

                                break;
                        }

                        /* Run the exit target if there is one, if not, just exit. */
                        if (manager_start_target(m, SPECIAL_EXIT_TARGET, JOB_REPLACE) < 0) {
                                m->exit_code = MANAGER_EXIT;
                                return 0;
                        }

                        break;

                case SIGWINCH:
                        if (MANAGER_IS_SYSTEM(m))
                                manager_start_target(m, SPECIAL_KBREQUEST_TARGET, JOB_REPLACE);

                        /* This is a nop on non-init */
                        break;

                case SIGPWR:
                        if (MANAGER_IS_SYSTEM(m))
                                manager_start_target(m, SPECIAL_SIGPWR_TARGET, JOB_REPLACE);

                        /* This is a nop on non-init */
                        break;

                case SIGUSR1: {
                        Unit *u;

                        u = manager_get_unit(m, SPECIAL_DBUS_SERVICE);

                        if (!u || UNIT_IS_ACTIVE_OR_RELOADING(unit_active_state(u))) {
                                log_info("Trying to reconnect to bus...");
                                bus_init(m, true);
                        }

                        if (!u || !UNIT_IS_ACTIVE_OR_ACTIVATING(unit_active_state(u))) {
                                log_info("Loading D-Bus service...");
                                manager_start_target(m, SPECIAL_DBUS_SERVICE, JOB_REPLACE);
                        }

                        break;
                }

                case SIGUSR2: {
                        _cleanup_free_ char *dump = NULL;
                        _cleanup_fclose_ FILE *f = NULL;
                        size_t size;

                        f = open_memstream(&dump, &size);
                        if (!f) {
                                log_warning_errno(errno, "Failed to allocate memory stream: %m");
                                break;
                        }

                        manager_dump_units(m, f, "\t");
                        manager_dump_jobs(m, f, "\t");

                        r = fflush_and_check(f);
                        if (r < 0) {
                                log_warning_errno(r, "Failed to write status stream: %m");
                                break;
                        }

                        log_dump(LOG_INFO, dump);
                        break;
                }

                case SIGHUP:
                        m->exit_code = MANAGER_RELOAD;
                        break;

                default: {

                        /* Starting SIGRTMIN+0 */
                        static const char * const target_table[] = {
                                [0] = SPECIAL_DEFAULT_TARGET,
                                [1] = SPECIAL_RESCUE_TARGET,
                                [2] = SPECIAL_EMERGENCY_TARGET,
                                [3] = SPECIAL_HALT_TARGET,
                                [4] = SPECIAL_POWEROFF_TARGET,
                                [5] = SPECIAL_REBOOT_TARGET,
                                [6] = SPECIAL_KEXEC_TARGET
                        };

                        /* Starting SIGRTMIN+13, so that target halt and system halt are 10 apart */
                        static const ManagerExitCode code_table[] = {
                                [0] = MANAGER_HALT,
                                [1] = MANAGER_POWEROFF,
                                [2] = MANAGER_REBOOT,
                                [3] = MANAGER_KEXEC
                        };

                        if ((int) sfsi.ssi_signo >= SIGRTMIN+0 &&
                            (int) sfsi.ssi_signo < SIGRTMIN+(int) ELEMENTSOF(target_table)) {
                                int idx = (int) sfsi.ssi_signo - SIGRTMIN;
                                manager_start_target(m, target_table[idx],
                                                     (idx == 1 || idx == 2) ? JOB_ISOLATE : JOB_REPLACE);
                                break;
                        }

                        if ((int) sfsi.ssi_signo >= SIGRTMIN+13 &&
                            (int) sfsi.ssi_signo < SIGRTMIN+13+(int) ELEMENTSOF(code_table)) {
                                m->exit_code = code_table[sfsi.ssi_signo - SIGRTMIN - 13];
                                break;
                        }

                        switch (sfsi.ssi_signo - SIGRTMIN) {

                        case 20:
                                manager_set_show_status(m, SHOW_STATUS_YES);
                                break;

                        case 21:
                                manager_set_show_status(m, SHOW_STATUS_NO);
                                break;

                        case 22:
                                log_set_max_level(LOG_DEBUG);
                                log_info("Setting log level to debug.");
                                break;

                        case 23:
                                log_set_max_level(LOG_INFO);
                                log_info("Setting log level to info.");
                                break;

                        case 24:
                                if (MANAGER_IS_USER(m)) {
                                        m->exit_code = MANAGER_EXIT;
                                        return 0;
                                }

                                /* This is a nop on init */
                                break;

                        case 26:
                        case 29: /* compatibility: used to be mapped to LOG_TARGET_SYSLOG_OR_KMSG */
                                log_set_target(LOG_TARGET_JOURNAL_OR_KMSG);
                                log_notice("Setting log target to journal-or-kmsg.");
                                break;

                        case 27:
                                log_set_target(LOG_TARGET_CONSOLE);
                                log_notice("Setting log target to console.");
                                break;

                        case 28:
                                log_set_target(LOG_TARGET_KMSG);
                                log_notice("Setting log target to kmsg.");
                                break;

                        default:
                                log_warning("Got unhandled signal <%s>.", signal_to_string(sfsi.ssi_signo));
                        }
                }
                }
        }

        if (sigchld)
                manager_dispatch_sigchld(m);

        return 0;
}
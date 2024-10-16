_public_ PAM_EXTERN int pam_sm_open_session(
                pam_handle_t *handle,
                int flags,
                int argc, const char **argv) {

        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        _cleanup_(sd_bus_message_unrefp) sd_bus_message *m = NULL, *reply = NULL;
        const char
                *username, *id, *object_path, *runtime_path,
                *service = NULL,
                *tty = NULL, *display = NULL,
                *remote_user = NULL, *remote_host = NULL,
                *seat = NULL,
                *type = NULL, *class = NULL,
                *class_pam = NULL, *type_pam = NULL, *cvtnr = NULL, *desktop = NULL, *desktop_pam = NULL,
                *memory_max = NULL, *tasks_max = NULL, *cpu_weight = NULL, *io_weight = NULL;
        _cleanup_(sd_bus_flush_close_unrefp) sd_bus *bus = NULL;
        int session_fd = -1, existing, r;
        bool debug = false, remote;
        struct passwd *pw;
        uint32_t vtnr = 0;
        uid_t original_uid;

        assert(handle);

        /* Make this a NOP on non-logind systems */
        if (!logind_running())
                return PAM_SUCCESS;

        if (parse_argv(handle,
                       argc, argv,
                       &class_pam,
                       &type_pam,
                       &desktop_pam,
                       &debug) < 0)
                return PAM_SESSION_ERR;

        if (debug)
                pam_syslog(handle, LOG_DEBUG, "pam-systemd initializing");

        r = get_user_data(handle, &username, &pw);
        if (r != PAM_SUCCESS) {
                pam_syslog(handle, LOG_ERR, "Failed to get user data.");
                return r;
        }

        /* Make sure we don't enter a loop by talking to
         * systemd-logind when it is actually waiting for the
         * background to finish start-up. If the service is
         * "systemd-user" we simply set XDG_RUNTIME_DIR and
         * leave. */

        pam_get_item(handle, PAM_SERVICE, (const void**) &service);
        if (streq_ptr(service, "systemd-user")) {
                char rt[STRLEN("/run/user/") + DECIMAL_STR_MAX(uid_t)];

                xsprintf(rt, "/run/user/"UID_FMT, pw->pw_uid);
                if (validate_runtime_directory(handle, rt, pw->pw_uid)) {
                        r = pam_misc_setenv(handle, "XDG_RUNTIME_DIR", rt, 0);
                        if (r != PAM_SUCCESS) {
                                pam_syslog(handle, LOG_ERR, "Failed to set runtime dir.");
                                return r;
                        }
                }

                r = export_legacy_dbus_address(handle, pw->pw_uid, rt);
                if (r != PAM_SUCCESS)
                        return r;

                return PAM_SUCCESS;
        }

        /* Otherwise, we ask logind to create a session for us */

        pam_get_item(handle, PAM_XDISPLAY, (const void**) &display);
        pam_get_item(handle, PAM_TTY, (const void**) &tty);
        pam_get_item(handle, PAM_RUSER, (const void**) &remote_user);
        pam_get_item(handle, PAM_RHOST, (const void**) &remote_host);

        seat = getenv_harder(handle, "XDG_SEAT", NULL);
        cvtnr = getenv_harder(handle, "XDG_VTNR", NULL);
        type = getenv_harder(handle, "XDG_SESSION_TYPE", type_pam);
        class = getenv_harder(handle, "XDG_SESSION_CLASS", class_pam);
        desktop = getenv_harder(handle, "XDG_SESSION_DESKTOP", desktop_pam);

        tty = strempty(tty);

        if (strchr(tty, ':')) {
                /* A tty with a colon is usually an X11 display, placed there to show up in utmp. We rearrange things
                 * and don't pretend that an X display was a tty. */
                if (isempty(display))
                        display = tty;
                tty = NULL;

        } else if (streq(tty, "cron")) {
                /* cron is setting PAM_TTY to "cron" for some reason (the commit carries no information why, but
                 * probably because it wants to set it to something as pam_time/pam_access/… require PAM_TTY to be set
                 * (as they otherwise even try to update it!) — but cron doesn't actually allocate a TTY for its forked
                 * off processes.) */
                type = "unspecified";
                class = "background";
                tty = NULL;

        } else if (streq(tty, "ssh")) {
                /* ssh has been setting PAM_TTY to "ssh" (for the same reason as cron does this, see above. For further
                 * details look for "PAM_TTY_KLUDGE" in the openssh sources). */
                type ="tty";
                class = "user";
                tty = NULL; /* This one is particularly sad, as this means that ssh sessions — even though usually
                             * associated with a pty — won't be tracked by their tty in logind. This is because ssh
                             * does the PAM session registration early for new connections, and registers a pty only
                             * much later (this is because it doesn't know yet if it needs one at all, as whether to
                             * register a pty or not is negotiated much later in the protocol). */

        } else
                /* Chop off leading /dev prefix that some clients specify, but others do not. */
                tty = skip_dev_prefix(tty);

        /* If this fails vtnr will be 0, that's intended */
        if (!isempty(cvtnr))
                (void) safe_atou32(cvtnr, &vtnr);

        if (!isempty(display) && !vtnr) {
                if (isempty(seat))
                        (void) get_seat_from_display(display, &seat, &vtnr);
                else if (streq(seat, "seat0"))
                        (void) get_seat_from_display(display, NULL, &vtnr);
        }

        if (seat && !streq(seat, "seat0") && vtnr != 0) {
                if (debug)
                        pam_syslog(handle, LOG_DEBUG, "Ignoring vtnr %"PRIu32" for %s which is not seat0", vtnr, seat);
                vtnr = 0;
        }

        if (isempty(type))
                type = !isempty(display) ? "x11" :
                           !isempty(tty) ? "tty" : "unspecified";

        if (isempty(class))
                class = streq(type, "unspecified") ? "background" : "user";

        remote = !isempty(remote_host) && !is_localhost(remote_host);

        (void) pam_get_data(handle, "systemd.memory_max", (const void **)&memory_max);
        (void) pam_get_data(handle, "systemd.tasks_max",  (const void **)&tasks_max);
        (void) pam_get_data(handle, "systemd.cpu_weight", (const void **)&cpu_weight);
        (void) pam_get_data(handle, "systemd.io_weight",  (const void **)&io_weight);

        /* Talk to logind over the message bus */

        r = sd_bus_open_system(&bus);
        if (r < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to connect to system bus: %s", strerror(-r));
                return PAM_SESSION_ERR;
        }

        if (debug) {
                pam_syslog(handle, LOG_DEBUG, "Asking logind to create session: "
                           "uid="UID_FMT" pid="PID_FMT" service=%s type=%s class=%s desktop=%s seat=%s vtnr=%"PRIu32" tty=%s display=%s remote=%s remote_user=%s remote_host=%s",
                           pw->pw_uid, getpid_cached(),
                           strempty(service),
                           type, class, strempty(desktop),
                           strempty(seat), vtnr, strempty(tty), strempty(display),
                           yes_no(remote), strempty(remote_user), strempty(remote_host));
                pam_syslog(handle, LOG_DEBUG, "Session limits: "
                           "memory_max=%s tasks_max=%s cpu_weight=%s io_weight=%s",
                           strna(memory_max), strna(tasks_max), strna(cpu_weight), strna(io_weight));
        }

        r = sd_bus_message_new_method_call(
                        bus,
                        &m,
                        "org.freedesktop.login1",
                        "/org/freedesktop/login1",
                        "org.freedesktop.login1.Manager",
                        "CreateSession");
        if (r < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to create CreateSession method call: %s", strerror(-r));
                return PAM_SESSION_ERR;
        }

        r = sd_bus_message_append(m, "uusssssussbss",
                        (uint32_t) pw->pw_uid,
                        0,
                        service,
                        type,
                        class,
                        desktop,
                        seat,
                        vtnr,
                        tty,
                        display,
                        remote,
                        remote_user,
                        remote_host);
        if (r < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to append to bus message: %s", strerror(-r));
                return PAM_SESSION_ERR;
        }

        r = sd_bus_message_open_container(m, 'a', "(sv)");
        if (r < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to open message container: %s", strerror(-r));
                return PAM_SYSTEM_ERR;
        }

        r = append_session_memory_max(handle, m, memory_max);
        if (r < 0)
                return PAM_SESSION_ERR;

        r = append_session_tasks_max(handle, m, tasks_max);
        if (r < 0)
                return PAM_SESSION_ERR;

        r = append_session_cg_weight(handle, m, cpu_weight, "CPUWeight");
        if (r < 0)
                return PAM_SESSION_ERR;

        r = append_session_cg_weight(handle, m, io_weight, "IOWeight");
        if (r < 0)
                return PAM_SESSION_ERR;

        r = sd_bus_message_close_container(m);
        if (r < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to close message container: %s", strerror(-r));
                return PAM_SYSTEM_ERR;
        }

        r = sd_bus_call(bus, m, 0, &error, &reply);
        if (r < 0) {
                if (sd_bus_error_has_name(&error, BUS_ERROR_SESSION_BUSY)) {
                        if (debug)
                                pam_syslog(handle, LOG_DEBUG, "Not creating session: %s", bus_error_message(&error, r));
                        return PAM_SUCCESS;
                } else {
                        pam_syslog(handle, LOG_ERR, "Failed to create session: %s", bus_error_message(&error, r));
                        return PAM_SYSTEM_ERR;
                }
        }

        r = sd_bus_message_read(reply,
                                "soshusub",
                                &id,
                                &object_path,
                                &runtime_path,
                                &session_fd,
                                &original_uid,
                                &seat,
                                &vtnr,
                                &existing);
        if (r < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to parse message: %s", strerror(-r));
                return PAM_SESSION_ERR;
        }

        if (debug)
                pam_syslog(handle, LOG_DEBUG, "Reply from logind: "
                           "id=%s object_path=%s runtime_path=%s session_fd=%d seat=%s vtnr=%u original_uid=%u",
                           id, object_path, runtime_path, session_fd, seat, vtnr, original_uid);

        r = update_environment(handle, "XDG_SESSION_ID", id);
        if (r != PAM_SUCCESS)
                return r;

        if (original_uid == pw->pw_uid) {
                /* Don't set $XDG_RUNTIME_DIR if the user we now
                 * authenticated for does not match the original user
                 * of the session. We do this in order not to result
                 * in privileged apps clobbering the runtime directory
                 * unnecessarily. */

                if (validate_runtime_directory(handle, runtime_path, pw->pw_uid)) {
                        r = update_environment(handle, "XDG_RUNTIME_DIR", runtime_path);
                        if (r != PAM_SUCCESS)
                                return r;
                }

                r = export_legacy_dbus_address(handle, pw->pw_uid, runtime_path);
                if (r != PAM_SUCCESS)
                        return r;
        }

        /* Most likely we got the session/type/class from environment variables, but might have gotten the data
         * somewhere else (for example PAM module parameters). Let's now update the environment variables, so that this
         * data is inherited into the session processes, and programs can rely on them to be initialized. */

        r = update_environment(handle, "XDG_SESSION_TYPE", type);
        if (r != PAM_SUCCESS)
                return r;

        r = update_environment(handle, "XDG_SESSION_CLASS", class);
        if (r != PAM_SUCCESS)
                return r;

        r = update_environment(handle, "XDG_SESSION_DESKTOP", desktop);
        if (r != PAM_SUCCESS)
                return r;

        r = update_environment(handle, "XDG_SEAT", seat);
        if (r != PAM_SUCCESS)
                return r;

        if (vtnr > 0) {
                char buf[DECIMAL_STR_MAX(vtnr)];
                sprintf(buf, "%u", vtnr);

                r = update_environment(handle, "XDG_VTNR", buf);
                if (r != PAM_SUCCESS)
                        return r;
        }

        r = pam_set_data(handle, "systemd.existing", INT_TO_PTR(!!existing), NULL);
        if (r != PAM_SUCCESS) {
                pam_syslog(handle, LOG_ERR, "Failed to install existing flag.");
                return r;
        }

        if (session_fd >= 0) {
                session_fd = fcntl(session_fd, F_DUPFD_CLOEXEC, 3);
                if (session_fd < 0) {
                        pam_syslog(handle, LOG_ERR, "Failed to dup session fd: %m");
                        return PAM_SESSION_ERR;
                }

                r = pam_set_data(handle, "systemd.session-fd", FD_TO_PTR(session_fd), NULL);
                if (r != PAM_SUCCESS) {
                        pam_syslog(handle, LOG_ERR, "Failed to install session fd.");
                        safe_close(session_fd);
                        return r;
                }
        }

        return PAM_SUCCESS;
}
char *session_info_session_for_pid(struct session_info *si, uint32_t pid)
{
    int i;
    int r;
    GStrv sessions = NULL;
    char *session = NULL;
    uid_t uid;

    r = sd_pid_get_session(pid, &session);
    if (r >= 0) {
        goto out;
    }

    /* If we could not get a session for the pid then the agent is probably
     * running in a systemd managed session. In that case we simply assume
     * it is actually part of the newest graphical session we can find. */
    r = sd_pid_get_owner_uid(pid, &uid);
    if (r < 0) {
        syslog(LOG_ERR, "Error getting owner UID for pid %u: %s",
               pid, strerror(-r));
        goto out;
    }

    r = sd_uid_get_sessions(uid, 0, &sessions);
    if (r < 0) {
        syslog(LOG_ERR, "Error getting sessions for UID %d: %s",
               (int) uid, strerror(-r));
        goto out;
    }

    for (i = 0; sessions[i] != NULL; i++) {
        char *session_type = NULL;

        r = sd_session_get_type(sessions[i], &session_type);

        if (r < 0) {
            syslog(LOG_ERR, "Error getting session type for session %s: %s",
                   sessions[i], strerror(-r));
            continue;
        }

        if (g_strcmp0(session_type, "wayland") == 0 ||
            g_strcmp0(session_type, "x11") == 0 ||
            g_strcmp0(session_type, "mir") == 0) {

            /* We prefer the newest session (i.e. last entry) from the
             * user, assuming that any old session that still exist has
             * just not yet died properly. */
            if (session != NULL)
                free (session);
            session = g_strdup(sessions[i]);
        }

        free(session_type);
    }

    g_strfreev(sessions);

out:
    if (session != NULL && si->verbose) {
        syslog(LOG_INFO, "Session for pid %u: %s", pid, session);
    }

    return session;
}
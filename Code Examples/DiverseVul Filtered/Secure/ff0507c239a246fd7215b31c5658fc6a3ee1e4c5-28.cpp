static void iscsi_parse_iscsi_option(const char *target, QDict *options)
{
    QemuOptsList *list;
    QemuOpts *opts;
    const char *user, *password, *password_secret, *initiator_name,
               *header_digest, *timeout;

    list = qemu_find_opts("iscsi");
    if (!list) {
        return;
    }

    opts = qemu_opts_find(list, target);
    if (opts == NULL) {
        opts = QTAILQ_FIRST(&list->head);
        if (!opts) {
            return;
        }
    }

    user = qemu_opt_get(opts, "user");
    if (user) {
        qdict_set_default_str(options, "user", user);
    }

    password = qemu_opt_get(opts, "password");
    if (password) {
        qdict_set_default_str(options, "password", password);
    }

    password_secret = qemu_opt_get(opts, "password-secret");
    if (password_secret) {
        qdict_set_default_str(options, "password-secret", password_secret);
    }

    initiator_name = qemu_opt_get(opts, "initiator-name");
    if (initiator_name) {
        qdict_set_default_str(options, "initiator-name", initiator_name);
    }

    header_digest = qemu_opt_get(opts, "header-digest");
    if (header_digest) {
        /* -iscsi takes upper case values, but QAPI only supports lower case
         * enum constant names, so we have to convert here. */
        char *qapi_value = g_ascii_strdown(header_digest, -1);
        qdict_set_default_str(options, "header-digest", qapi_value);
        g_free(qapi_value);
    }

    timeout = qemu_opt_get(opts, "timeout");
    if (timeout) {
        qdict_set_default_str(options, "timeout", timeout);
    }
}
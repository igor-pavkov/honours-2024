static void wsgi_setup_daemon_name(WSGIDaemonProcess *daemon, apr_pool_t *p)
{
    const char *display_name = NULL;

#if !(defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__))
    int slen = 0;
    int dlen = 0;

    char *argv0 = NULL;
#endif

    display_name = daemon->group->display_name;

    if (!display_name)
        return;

    if (!strcmp(display_name, "%{GROUP}")) {
        display_name = apr_pstrcat(p, "(wsgi:", daemon->group->name,
                                   ")", NULL);
    }

    /*
     * Only argv[0] is guaranteed to be the real things as MPM
     * modules may make modifications to subsequent arguments.
     * Thus can only replace the argv[0] value. Because length
     * is restricted, need to truncate display name if too long.
     */

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
    setproctitle("%s", display_name);
#else
    argv0 = (char*)wsgi_server->process->argv[0];

    dlen = strlen(argv0);
    slen = strlen(display_name);

    memset(argv0, ' ', dlen);

    if (slen < dlen)
        memcpy(argv0, display_name, slen);
    else
        memcpy(argv0, display_name, dlen);
#endif
}
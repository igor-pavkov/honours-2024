static void dodaemonize(void)
{
    pid_t child;
    unsigned int i;

    /* Contributed by Jason Lunz - also based on APUI code, see open_max() */
    if (daemonize != 0) {
        if ((child = fork()) == (pid_t) -1) {
            perror(MSG_STANDALONE_FAILED " - fork");
            logfile(LOG_ERR, MSG_STANDALONE_FAILED ": [fork: %s]", strerror(errno));
            return;
        } else if (child != (pid_t) 0) {
            _EXIT(EXIT_SUCCESS);       /* parent exits */
        }
        if (setsid() == (pid_t) -1) {
            perror(MSG_STANDALONE_FAILED " - setsid");   /* continue anyway */
        }
# ifndef NON_ROOT_FTP
        if (chdir("/") != 0) {
            perror("chdir");
            _EXIT(EXIT_FAILURE);
        }
# endif
        i = open_max();
        do {
            if (isatty((int) i)) {
                (void) close((int) i);
            }
            i--;
        } while (i > 2U);
        if (closedesc_all(1) != 0) {
            perror(MSG_STANDALONE_FAILED " - /dev/null duplication");
            _EXIT(EXIT_FAILURE);
        }
    }
}
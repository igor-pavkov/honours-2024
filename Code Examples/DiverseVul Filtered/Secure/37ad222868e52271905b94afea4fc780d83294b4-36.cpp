static void clearargs(int argc, char **argv)
{
#ifndef NO_PROCNAME_CHANGE
# if defined(__linux__) && !defined(HAVE_SETPROCTITLE)
    int i;
    char *first = NULL;
    char *next = NULL;

    for (i = 0; i < argc; i++) {
        if (first == NULL) {
            first = argv[i];
        }
        if (next == NULL || argv[i] == next + 1) {
            next = argv[i] + strlen(argv[i]);
        }
    }
    for (i = 0; environ[i] != NULL; i++) {
        if (first == NULL) {
            first = argv[i];
        }
        if (next == NULL) {
            next = argv[i] + strlen(argv[i]);
        }
    }
    if (first == NULL || next == NULL) {
        return;
    }
    argv_lth = next - first;
    argv0 = argv;
    if (environ != NULL) {
        char **new_environ;
        unsigned int env_nb = 0U;

        while (environ[env_nb] != NULL) {
            env_nb++;
        }
        if ((new_environ = malloc((1U + env_nb) * sizeof (char *))) == NULL) {
            abort();
        }
        new_environ[env_nb] = NULL;
        while (env_nb > 0U) {
            env_nb--;
            new_environ[env_nb] = strdup(environ[env_nb]);
        }
        environ = new_environ;
    }
# else
    (void) argc;
    (void) argv;
# endif
#endif
}
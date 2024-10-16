static void set_signals(void)
{
#ifndef NO_STANDALONE
    sigset_t sigs;
    struct sigaction sa;

    sigfillset(&sigs);
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigchild;
    sigdelset(&sigs, SIGCHLD);
    (void) sigaction(SIGCHLD, &sa, NULL);

    sa.sa_handler = SIG_IGN;
    (void) sigaction(SIGPIPE, &sa, NULL);
    (void) sigaction(SIGALRM, &sa, NULL);
    (void) sigaction(SIGURG, &sa, NULL);
#ifdef SIGIO
    (void) sigaction(SIGIO, &sa, NULL);
#endif

    sa.sa_flags = 0;
    sa.sa_handler = sigterm;
    sigdelset(&sigs, SIGTERM);
    (void) sigaction(SIGTERM, &sa, NULL);
    sigdelset(&sigs, SIGHUP);
    (void) sigaction(SIGHUP, &sa, NULL);
    sigdelset(&sigs, SIGQUIT);
    (void) sigaction(SIGQUIT, &sa, NULL);
    sigdelset(&sigs, SIGINT);
    (void) sigaction(SIGINT, &sa, NULL);
# ifdef SIGXCPU
    sigdelset(&sigs, SIGXCPU);
    (void) sigaction(SIGXCPU, &sa, NULL);
# endif
    (void) sigprocmask(SIG_SETMASK, &sigs, NULL);
#endif
}
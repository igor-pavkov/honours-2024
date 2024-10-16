static void set_signals_client(void)
{
    sigset_t sigs;
    struct sigaction sa;

    sigfillset(&sigs);
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;

    sa.sa_handler = SIG_IGN;
    (void) sigaction(SIGPIPE, &sa, NULL);
    (void) sigaction(SIGURG, &sa, NULL);
#ifdef SIGIO
    (void) sigaction(SIGIO, &sa, NULL);
#endif

    sa.sa_handler = SIG_DFL;
    sigdelset(&sigs, SIGCHLD);
    (void) sigaction(SIGCHLD, &sa, NULL);
#ifdef SIGFPE
    (void) sigaction(SIGFPE, &sa, NULL);
    sigdelset(&sigs, SIGFPE);
#endif
    sa.sa_flags = 0;

    sa.sa_handler = sigalarm;
    sigdelset(&sigs, SIGALRM);
    (void) sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = sigterm_client;
    sigdelset(&sigs, SIGTERM);
    (void) sigaction(SIGTERM, &sa, NULL);
    sigdelset(&sigs, SIGHUP);
    (void) sigaction(SIGHUP, &sa, NULL);
    sigdelset(&sigs, SIGQUIT);
    (void) sigaction(SIGQUIT, &sa, NULL);
    sigdelset(&sigs, SIGINT);
    (void) sigaction(SIGINT, &sa, NULL);
#ifdef SIGXCPU
    sigdelset(&sigs, SIGXCPU);
    (void) sigaction(SIGXCPU, &sa, NULL);
#endif
    (void) sigprocmask(SIG_SETMASK, &sigs, NULL);
}
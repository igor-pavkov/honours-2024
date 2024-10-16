void disablesignals(void)
{
    sigset_t sigs;

    sigfillset(&sigs);
    if (sigprocmask(SIG_BLOCK, &sigs, &old_sigmask) < 0) {
        _EXIT(EXIT_FAILURE);
    }
}
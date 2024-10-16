static void enablesignals(void)
{
    if (sigprocmask(SIG_SETMASK, &old_sigmask, NULL) < 0) {
        _EXIT(EXIT_FAILURE);
    }
}
static void sigalarm(int sig)
{
    (void) sig;
    disablesignals();
    die(421, LOG_INFO, MSG_TIMEOUT);
}
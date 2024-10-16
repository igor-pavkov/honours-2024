static void sigterm_client(int sig)
{
    (void) sig;

    disablesignals();
    _EXIT(EXIT_SUCCESS);
}
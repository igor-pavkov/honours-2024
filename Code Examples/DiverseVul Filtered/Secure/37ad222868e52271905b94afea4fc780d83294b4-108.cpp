static void fixlimits(void)
{
#ifdef HAVE_SETRLIMIT
    static struct rlimit lim;

    lim.rlim_max = lim.rlim_cur = MAX_CPU_TIME;
    setrlimit(RLIMIT_CPU, &lim);
    lim.rlim_max = lim.rlim_cur = MAX_DATA_SIZE;
    setrlimit(RLIMIT_DATA, &lim);
# ifndef DEBUG
    lim.rlim_max = lim.rlim_cur = 0;
    setrlimit(RLIMIT_CORE, &lim);
# endif
#endif
}
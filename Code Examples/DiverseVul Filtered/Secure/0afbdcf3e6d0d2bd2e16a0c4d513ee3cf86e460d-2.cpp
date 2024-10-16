OPJ_FLOAT64 opj_clock(void)
{
#ifdef _WIN32
    /* _WIN32: use QueryPerformance (very accurate) */
    LARGE_INTEGER freq, t ;
    /* freq is the clock speed of the CPU */
    QueryPerformanceFrequency(&freq) ;
    /* cout << "freq = " << ((double) freq.QuadPart) << endl; */
    /* t is the high resolution performance counter (see MSDN) */
    QueryPerformanceCounter(& t) ;
    return freq.QuadPart ? ((OPJ_FLOAT64)t.QuadPart / (OPJ_FLOAT64)freq.QuadPart) :
           0;
#elif defined(__linux)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((OPJ_FLOAT64)ts.tv_sec + (OPJ_FLOAT64)ts.tv_nsec * 1e-9);
#else
    /* Unix : use resource usage */
    /* FIXME: this counts the total CPU time, instead of the user perceived time */
    struct rusage t;
    OPJ_FLOAT64 procTime;
    /* (1) Get the rusage data structure at this moment (man getrusage) */
    getrusage(0, &t);
    /* (2) What is the elapsed time ? - CPU time = User time + System time */
    /* (2a) Get the seconds */
    procTime = (OPJ_FLOAT64)(t.ru_utime.tv_sec + t.ru_stime.tv_sec);
    /* (2b) More precisely! Get the microseconds part ! */
    return (procTime + (OPJ_FLOAT64)(t.ru_utime.tv_usec + t.ru_stime.tv_usec) *
            1e-6) ;
#endif
}
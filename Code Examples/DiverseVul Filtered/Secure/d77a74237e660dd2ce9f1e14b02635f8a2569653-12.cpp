void sys_srandom(unsigned int seed)
{
#if defined(HAVE_SRANDOM)
	srandom(seed);
#elif defined(HAVE_SRAND)
	srand(seed);
#else
	DEBUG(0,("Error - no srandom function available !\n"));
	exit(1);
#endif
}
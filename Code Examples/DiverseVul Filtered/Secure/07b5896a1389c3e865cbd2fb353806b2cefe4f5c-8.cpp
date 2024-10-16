static int set_verbose(void)
{
	verbose = !!getenv("GIF_VERBOSE");
	verbose_set = 1;
	return(verbose);
}
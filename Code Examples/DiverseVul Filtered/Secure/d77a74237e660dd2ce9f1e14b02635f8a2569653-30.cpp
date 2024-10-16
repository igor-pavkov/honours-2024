static struct timespec get_atimespec(const struct stat *pst)
{
#if !defined(HAVE_STAT_HIRES_TIMESTAMPS)
	struct timespec ret;

	/* Old system - no ns timestamp. */
	ret.tv_sec = pst->st_atime;
	ret.tv_nsec = 0;
	return ret;
#else
#if defined(HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC)
	struct timespec ret;
	ret.tv_sec = pst->st_atim.tv_sec;
	ret.tv_nsec = pst->st_atim.tv_nsec;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_MTIMENSEC)
	struct timespec ret;
	ret.tv_sec = pst->st_atime;
	ret.tv_nsec = pst->st_atimensec;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_MTIME_N)
	struct timespec ret;
	ret.tv_sec = pst->st_atime;
	ret.tv_nsec = pst->st_atime_n;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_UMTIME)
	struct timespec ret;
	ret.tv_sec = pst->st_atime;
	ret.tv_nsec = pst->st_uatime * 1000;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC)
	return pst->st_atimespec;
#else
#error	CONFIGURE_ERROR_IN_DETECTING_TIMESPEC_IN_STAT
#endif
#endif
}
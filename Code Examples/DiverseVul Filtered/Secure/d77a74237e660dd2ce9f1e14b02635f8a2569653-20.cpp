static struct timespec get_mtimespec(const struct stat *pst)
{
#if !defined(HAVE_STAT_HIRES_TIMESTAMPS)
	struct timespec ret;

	/* Old system - no ns timestamp. */
	ret.tv_sec = pst->st_mtime;
	ret.tv_nsec = 0;
	return ret;
#else
#if defined(HAVE_STRUCT_STAT_ST_MTIM_TV_NSEC)
	struct timespec ret;
	ret.tv_sec = pst->st_mtim.tv_sec;
	ret.tv_nsec = pst->st_mtim.tv_nsec;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_MTIMENSEC)
	struct timespec ret;
	ret.tv_sec = pst->st_mtime;
	ret.tv_nsec = pst->st_mtimensec;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_MTIME_N)
	struct timespec ret;
	ret.tv_sec = pst->st_mtime;
	ret.tv_nsec = pst->st_mtime_n;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_UMTIME)
	struct timespec ret;
	ret.tv_sec = pst->st_mtime;
	ret.tv_nsec = pst->st_umtime * 1000;
	return ret;
#elif defined(HAVE_STRUCT_STAT_ST_MTIMESPEC_TV_NSEC)
	return pst->st_mtimespec;
#else
#error	CONFIGURE_ERROR_IN_DETECTING_TIMESPEC_IN_STAT
#endif
#endif
}
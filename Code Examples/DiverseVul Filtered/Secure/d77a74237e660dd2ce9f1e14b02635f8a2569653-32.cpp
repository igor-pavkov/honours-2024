static struct timespec calc_create_time_stat_ex(const struct stat_ex *st)
{
	struct timespec ret, ret1;
	struct timespec c_time = st->st_ex_ctime;
	struct timespec m_time = st->st_ex_mtime;
	struct timespec a_time = st->st_ex_atime;

	ret = timespec_compare(&c_time, &m_time) < 0 ? c_time : m_time;
	ret1 = timespec_compare(&ret, &a_time) < 0 ? ret : a_time;

	if(!null_timespec(ret1)) {
		return ret1;
	}

	/*
	 * One of ctime, mtime or atime was zero (probably atime).
	 * Just return MIN(ctime, mtime).
	 */
	return ret;
}
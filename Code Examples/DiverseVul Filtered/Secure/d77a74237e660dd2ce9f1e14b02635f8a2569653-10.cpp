static struct timespec calc_create_time_stat(const struct stat *st)
{
	struct timespec ret, ret1;
	struct timespec c_time = get_ctimespec(st);
	struct timespec m_time = get_mtimespec(st);
	struct timespec a_time = get_atimespec(st);

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
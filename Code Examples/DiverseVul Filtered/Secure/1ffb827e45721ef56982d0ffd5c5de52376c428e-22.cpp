unsigned int timespec_sub_ms(struct timespec *a, struct timespec *b)
{
	time_t dsecs;

	dsecs = a->tv_sec - b->tv_sec;
	if (!INT_MULTIPLY_OVERFLOW(dsecs, 1000)) {
		return (dsecs*1000 + (a->tv_nsec - b->tv_nsec) / (1000 * 1000));
	} else {
		return UINT_MAX;
	}
}
static inline void php_openssl_rand_add_timeval()  /* {{{ */
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	RAND_add(&tv, sizeof(tv), 0.0);
}
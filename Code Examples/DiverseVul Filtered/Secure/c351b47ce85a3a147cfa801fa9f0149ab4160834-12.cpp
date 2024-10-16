static int pcre_clean_cache(void *data, void *arg TSRMLS_DC)
{
	int *num_clean = (int *)arg;

	if (*num_clean > 0) {
		(*num_clean)--;
		return 1;
	} else {
		return 0;
	}
}
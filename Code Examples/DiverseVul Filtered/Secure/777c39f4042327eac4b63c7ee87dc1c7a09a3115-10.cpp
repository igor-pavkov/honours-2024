static int zend_shared_alloc_try(const zend_shared_memory_handler_entry *he, size_t requested_size, zend_shared_segment ***shared_segments_p, int *shared_segments_count, char **error_in)
{
	int res;
	g_shared_alloc_handler = he->handler;
	g_shared_model = he->name;
	ZSMMG(shared_segments) = NULL;
	ZSMMG(shared_segments_count) = 0;

	res = S_H(create_segments)(requested_size, shared_segments_p, shared_segments_count, error_in);

	if (res) {
		/* this model works! */
		return res;
	}
	if (*shared_segments_p) {
		int i;
		/* cleanup */
		for (i = 0; i < *shared_segments_count; i++) {
			if ((*shared_segments_p)[i]->p && (*shared_segments_p)[i]->p != (void *)-1) {
				S_H(detach_segment)((*shared_segments_p)[i]);
			}
		}
		free(*shared_segments_p);
		*shared_segments_p = NULL;
	}
	g_shared_alloc_handler = NULL;
	return ALLOC_FAILURE;
}
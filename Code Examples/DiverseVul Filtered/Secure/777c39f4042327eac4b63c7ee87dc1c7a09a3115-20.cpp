int zend_shared_alloc_startup(size_t requested_size)
{
	zend_shared_segment **tmp_shared_segments;
	size_t shared_segments_array_size;
	zend_smm_shared_globals tmp_shared_globals, *p_tmp_shared_globals;
	char *error_in = NULL;
	const zend_shared_memory_handler_entry *he;
	int res = ALLOC_FAILURE;


	/* shared_free must be valid before we call zend_shared_alloc()
	 * - make it temporarily point to a local variable
	 */
	smm_shared_globals = &tmp_shared_globals;
	ZSMMG(shared_free) = requested_size; /* goes to tmp_shared_globals.shared_free */

	zend_shared_alloc_create_lock();

	if (ZCG(accel_directives).memory_model && ZCG(accel_directives).memory_model[0]) {
		char *model = ZCG(accel_directives).memory_model;
		/* "cgi" is really "shm"... */
		if (strncmp(ZCG(accel_directives).memory_model, "cgi", sizeof("cgi")) == 0) {
			model = "shm";
		}

		for (he = handler_table; he->name; he++) {
			if (strcmp(model, he->name) == 0) {
				res = zend_shared_alloc_try(he, requested_size, &ZSMMG(shared_segments), &ZSMMG(shared_segments_count), &error_in);
				if (res) {
					/* this model works! */
				}
				break;
			}
		}
	}

	if (res == FAILED_REATTACHED) {
		smm_shared_globals = NULL;
		return res;
	}

	if (!g_shared_alloc_handler) {
		/* try memory handlers in order */
		for (he = handler_table; he->name; he++) {
			res = zend_shared_alloc_try(he, requested_size, &ZSMMG(shared_segments), &ZSMMG(shared_segments_count), &error_in);
			if (res) {
				/* this model works! */
				break;
			}
		}
	}

	if (!g_shared_alloc_handler) {
		no_memory_bailout(requested_size, error_in);
		return ALLOC_FAILURE;
	}

	if (res == SUCCESSFULLY_REATTACHED) {
		return res;
	}

	shared_segments_array_size = ZSMMG(shared_segments_count) * S_H(segment_type_size)();

	/* move shared_segments and shared_free to shared memory */
	ZCG(locked) = 1; /* no need to perform a real lock at this point */
	p_tmp_shared_globals = (zend_smm_shared_globals *) zend_shared_alloc(sizeof(zend_smm_shared_globals));
	if (!p_tmp_shared_globals) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;;
	}

	tmp_shared_segments = zend_shared_alloc(shared_segments_array_size + ZSMMG(shared_segments_count) * sizeof(void *));
	if (!tmp_shared_segments) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;;
	}

	copy_shared_segments(tmp_shared_segments, ZSMMG(shared_segments)[0], ZSMMG(shared_segments_count), S_H(segment_type_size)());

	*p_tmp_shared_globals = tmp_shared_globals;
	smm_shared_globals = p_tmp_shared_globals;

	free(ZSMMG(shared_segments));
	ZSMMG(shared_segments) = tmp_shared_segments;

	ZSMMG(shared_memory_state).positions = (int *)zend_shared_alloc(sizeof(int) * ZSMMG(shared_segments_count));
	if (!ZSMMG(shared_memory_state).positions) {
		zend_accel_error(ACCEL_LOG_FATAL, "Insufficient shared memory!");
		return ALLOC_FAILURE;;
	}

	ZCG(locked) = 0;

	return res;
}
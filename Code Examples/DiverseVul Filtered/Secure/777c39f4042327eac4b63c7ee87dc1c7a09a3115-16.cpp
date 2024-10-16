void zend_shared_alloc_shutdown(void)
{
	zend_shared_segment **tmp_shared_segments;
	size_t shared_segments_array_size;
	zend_smm_shared_globals tmp_shared_globals;
	int i;

	tmp_shared_globals = *smm_shared_globals;
	smm_shared_globals = &tmp_shared_globals;
	shared_segments_array_size = ZSMMG(shared_segments_count) * (S_H(segment_type_size)() + sizeof(void *));
	tmp_shared_segments = emalloc(shared_segments_array_size);
	copy_shared_segments(tmp_shared_segments, ZSMMG(shared_segments)[0], ZSMMG(shared_segments_count), S_H(segment_type_size)());
	ZSMMG(shared_segments) = tmp_shared_segments;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		S_H(detach_segment)(ZSMMG(shared_segments)[i]);
	}
	efree(ZSMMG(shared_segments));
	ZSMMG(shared_segments) = NULL;
	g_shared_alloc_handler = NULL;
#ifndef ZEND_WIN32
	close(lock_file);
#endif
}
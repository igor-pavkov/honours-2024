void *zend_shared_alloc(size_t size)
{
	int i;
	unsigned int block_size = ZEND_ALIGNED_SIZE(size);

#if 1
	if (!ZCG(locked)) {
		zend_accel_error(ACCEL_LOG_ERROR, "Shared memory lock not obtained");
	}
#endif
	if (block_size > ZSMMG(shared_free)) { /* No hope to find a big-enough block */
		SHARED_ALLOC_FAILED();
		return NULL;
	}
	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		if (ZSMMG(shared_segments)[i]->size - ZSMMG(shared_segments)[i]->pos >= block_size) { /* found a valid block */
			void *retval = (void *) (((char *) ZSMMG(shared_segments)[i]->p) + ZSMMG(shared_segments)[i]->pos);

			ZSMMG(shared_segments)[i]->pos += block_size;
			ZSMMG(shared_free) -= block_size;
			memset(retval, 0, block_size);
			return retval;
		}
	}
	SHARED_ALLOC_FAILED();
	return NULL;
}
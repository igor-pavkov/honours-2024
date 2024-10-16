static size_t zend_shared_alloc_get_largest_free_block(void)
{
	int i;
	size_t largest_block_size = 0;

	for (i = 0; i < ZSMMG(shared_segments_count); i++) {
		size_t block_size = ZSMMG(shared_segments)[i]->size - ZSMMG(shared_segments)[i]->pos;

		if (block_size>largest_block_size) {
			largest_block_size = block_size;
		}
	}
	return largest_block_size;
}
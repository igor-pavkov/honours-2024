static handle_t *new_handle(int nblocks)
{
	handle_t *handle = jbd2_alloc_handle(GFP_NOFS);
	if (!handle)
		return NULL;
	handle->h_buffer_credits = nblocks;
	handle->h_ref = 1;

	return handle;
}
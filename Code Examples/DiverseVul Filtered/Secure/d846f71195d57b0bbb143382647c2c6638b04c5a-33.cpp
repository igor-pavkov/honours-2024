static int ebt_buf_add(struct ebt_entries_buf_state *state,
		       void *data, unsigned int sz)
{
	if (state->buf_kern_start == NULL)
		goto count_only;

	BUG_ON(state->buf_kern_offset + sz > state->buf_kern_len);

	memcpy(state->buf_kern_start + state->buf_kern_offset, data, sz);

 count_only:
	state->buf_user_offset += sz;
	return ebt_buf_count(state, sz);
}
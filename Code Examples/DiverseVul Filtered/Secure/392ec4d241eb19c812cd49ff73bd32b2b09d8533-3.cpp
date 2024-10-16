static struct dns_server_tkey_store *tkey_store_init(TALLOC_CTX *mem_ctx,
						     uint16_t size)
{
	struct dns_server_tkey_store *buffer = talloc_zero(mem_ctx,
						struct dns_server_tkey_store);

	if (buffer == NULL) {
		return NULL;
	}

	buffer->size = size;
	buffer->next_idx = 0;

	buffer->tkeys = talloc_zero_array(buffer, struct dns_server_tkey *, size);
	if (buffer->tkeys == NULL) {
		TALLOC_FREE(buffer);
	}

	return buffer;
}
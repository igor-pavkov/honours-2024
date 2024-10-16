static struct ea_list *read_ea_list(TALLOC_CTX *ctx, const char *pdata, size_t data_size)
{
	struct ea_list *ea_list_head = NULL;
	size_t offset = 0;
	size_t bytes_used = 0;

	while (offset < data_size) {
		struct ea_list *eal = read_ea_list_entry(ctx, pdata + offset, data_size - offset, &bytes_used);

		if (!eal) {
			return NULL;
		}

		DLIST_ADD_END(ea_list_head, eal);
		offset += bytes_used;
	}

	return ea_list_head;
}
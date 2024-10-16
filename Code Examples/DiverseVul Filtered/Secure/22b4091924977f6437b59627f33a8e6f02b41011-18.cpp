static struct ea_list *read_ea_name_list(TALLOC_CTX *ctx, const char *pdata, size_t data_size)
{
	struct ea_list *ea_list_head = NULL;
	size_t converted_size, offset = 0;

	while (offset + 2 < data_size) {
		struct ea_list *eal = talloc_zero(ctx, struct ea_list);
		unsigned int namelen = CVAL(pdata,offset);

		offset++; /* Go past the namelen byte. */

		/* integer wrap paranioa. */
		if ((offset + namelen < offset) || (offset + namelen < namelen) ||
				(offset > data_size) || (namelen > data_size) ||
				(offset + namelen >= data_size)) {
			break;
		}
		/* Ensure the name is null terminated. */
		if (pdata[offset + namelen] != '\0') {
			return NULL;
		}
		if (!pull_ascii_talloc(ctx, &eal->ea.name, &pdata[offset],
				       &converted_size)) {
			DEBUG(0,("read_ea_name_list: pull_ascii_talloc "
				 "failed: %s", strerror(errno)));
		}
		if (!eal->ea.name) {
			return NULL;
		}

		offset += (namelen + 1); /* Go past the name + terminating zero. */
		DLIST_ADD_END(ea_list_head, eal);
		DEBUG(10,("read_ea_name_list: read ea name %s\n", eal->ea.name));
	}

	return ea_list_head;
}
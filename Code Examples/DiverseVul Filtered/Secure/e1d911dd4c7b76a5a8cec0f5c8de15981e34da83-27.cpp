static int find_tree_entry(struct tree_desc *t, const char *name, unsigned char *result, unsigned *mode)
{
	int namelen = strlen(name);
	while (t->size) {
		const char *entry;
		const struct object_id *oid;
		int entrylen, cmp;

		oid = tree_entry_extract(t, &entry, mode);
		entrylen = tree_entry_len(&t->entry);
		update_tree_entry(t);
		if (entrylen > namelen)
			continue;
		cmp = memcmp(name, entry, entrylen);
		if (cmp > 0)
			continue;
		if (cmp < 0)
			break;
		if (entrylen == namelen) {
			hashcpy(result, oid->hash);
			return 0;
		}
		if (name[entrylen] != '/')
			continue;
		if (!S_ISDIR(*mode))
			break;
		if (++entrylen == namelen) {
			hashcpy(result, oid->hash);
			return 0;
		}
		return get_tree_entry(oid->hash, name + entrylen, result, mode);
	}
	return -1;
}
static int update_tree_entry_internal(struct tree_desc *desc, struct strbuf *err)
{
	const void *buf = desc->buffer;
	const unsigned char *end = desc->entry.oid->hash + 20;
	unsigned long size = desc->size;
	unsigned long len = end - (const unsigned char *)buf;

	if (size < len)
		die(_("too-short tree file"));
	buf = end;
	size -= len;
	desc->buffer = buf;
	desc->size = size;
	if (size)
		return decode_tree_entry(desc, buf, size, err);
	return 0;
}
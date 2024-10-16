static int init_tree_desc_internal(struct tree_desc *desc, const void *buffer, unsigned long size, struct strbuf *err)
{
	desc->buffer = buffer;
	desc->size = size;
	if (size)
		return decode_tree_entry(desc, buffer, size, err);
	return 0;
}
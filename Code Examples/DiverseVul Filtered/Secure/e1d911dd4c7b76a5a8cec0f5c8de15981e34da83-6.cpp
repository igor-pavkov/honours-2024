int init_tree_desc_gently(struct tree_desc *desc, const void *buffer, unsigned long size)
{
	struct strbuf err = STRBUF_INIT;
	int result = init_tree_desc_internal(desc, buffer, size, &err);
	if (result)
		error("%s", err.buf);
	strbuf_release(&err);
	return result;
}
void init_tree_desc(struct tree_desc *desc, const void *buffer, unsigned long size)
{
	struct strbuf err = STRBUF_INIT;
	if (init_tree_desc_internal(desc, buffer, size, &err))
		die("%s", err.buf);
	strbuf_release(&err);
}
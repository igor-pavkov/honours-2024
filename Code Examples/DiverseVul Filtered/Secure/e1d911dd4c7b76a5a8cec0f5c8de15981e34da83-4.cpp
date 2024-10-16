void update_tree_entry(struct tree_desc *desc)
{
	struct strbuf err = STRBUF_INIT;
	if (update_tree_entry_internal(desc, &err))
		die("%s", err.buf);
	strbuf_release(&err);
}
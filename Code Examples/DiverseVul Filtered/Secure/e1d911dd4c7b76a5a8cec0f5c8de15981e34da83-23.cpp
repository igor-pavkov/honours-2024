int update_tree_entry_gently(struct tree_desc *desc)
{
	struct strbuf err = STRBUF_INIT;
	if (update_tree_entry_internal(desc, &err)) {
		error("%s", err.buf);
		strbuf_release(&err);
		/* Stop processing this tree after error */
		desc->size = 0;
		return -1;
	}
	strbuf_release(&err);
	return 0;
}
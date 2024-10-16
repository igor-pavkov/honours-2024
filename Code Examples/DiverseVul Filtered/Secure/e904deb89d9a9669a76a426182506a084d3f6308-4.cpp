static int gitmodule_oid_from_commit(const struct object_id *treeish_name,
				     struct object_id *gitmodules_oid,
				     struct strbuf *rev)
{
	int ret = 0;

	if (is_null_oid(treeish_name)) {
		oidclr(gitmodules_oid);
		return 1;
	}

	strbuf_addf(rev, "%s:.gitmodules", oid_to_hex(treeish_name));
	if (get_oid(rev->buf, gitmodules_oid) >= 0)
		ret = 1;

	return ret;
}
static inline int prune_traversal(struct name_entry *e,
				  struct traverse_info *info,
				  struct strbuf *base,
				  int still_interesting)
{
	if (!info->pathspec || still_interesting == 2)
		return 2;
	if (still_interesting < 0)
		return still_interesting;
	return tree_entry_interesting(e, base, 0, info->pathspec);
}
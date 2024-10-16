int tree_entry_gently(struct tree_desc *desc, struct name_entry *entry)
{
	if (!desc->size)
		return 0;

	*entry = desc->entry;
	if (update_tree_entry_gently(desc))
		return 0;
	return 1;
}
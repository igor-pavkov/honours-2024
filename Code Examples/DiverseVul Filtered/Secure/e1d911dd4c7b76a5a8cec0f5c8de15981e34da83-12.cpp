int tree_entry(struct tree_desc *desc, struct name_entry *entry)
{
	if (!desc->size)
		return 0;

	*entry = desc->entry;
	update_tree_entry(desc);
	return 1;
}
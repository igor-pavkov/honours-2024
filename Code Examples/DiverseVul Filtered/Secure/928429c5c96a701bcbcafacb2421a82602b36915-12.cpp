static int append_entry(
	git_treebuilder *bld,
	const char *filename,
	const git_oid *id,
	git_filemode_t filemode)
{
	git_tree_entry *entry;

	if (!valid_entry_name(filename))
		return tree_error("Failed to insert entry. Invalid name for a tree entry", filename);

	entry = alloc_entry(filename);
	GITERR_CHECK_ALLOC(entry);

	git_oid_cpy(&entry->oid, id);
	entry->attr = (uint16_t)filemode;

	if (git_vector_insert_sorted(&bld->entries, entry, NULL) < 0) {
		git__free(entry);
		return -1;
	}

	bld->entrycount++;
	return 0;
}
int git_treebuilder_insert(
	const git_tree_entry **entry_out,
	git_treebuilder *bld,
	const char *filename,
	const git_oid *id,
	git_filemode_t filemode)
{
	git_tree_entry *entry;
	size_t pos;

	assert(bld && id && filename);

	if (!valid_filemode(filemode))
		return tree_error("Failed to insert entry. Invalid filemode for file", filename);

	if (!valid_entry_name(filename))
		return tree_error("Failed to insert entry. Invalid name for a tree entry", filename);

	if (!tree_key_search(&pos, &bld->entries, filename, strlen(filename))) {
		entry = git_vector_get(&bld->entries, pos);
		if (entry->removed) {
			entry->removed = 0;
			bld->entrycount++;
		}

		entry->attr = filemode;
		git_oid_cpy(&entry->oid, id);
	} else {
		entry = alloc_entry(filename);
		GITERR_CHECK_ALLOC(entry);

		entry->attr = filemode;
		git_oid_cpy(&entry->oid, id);

		if (git_vector_insert_sorted(&bld->entries, entry, NULL) < 0) {
			git__free(entry);
			return -1;
		}

		bld->entrycount++;
	}

	if (entry_out)
		*entry_out = entry;

	return 0;
}
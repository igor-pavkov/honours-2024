void git_treebuilder_filter(
	git_treebuilder *bld,
	git_treebuilder_filter_cb filter,
	void *payload)
{
	size_t i;
	git_tree_entry *entry;

	assert(bld && filter);

	git_vector_foreach(&bld->entries, i, entry) {
		if (!entry->removed && filter(entry, payload)) {
			entry->removed = 1;
			bld->entrycount--;
		}
	}
}
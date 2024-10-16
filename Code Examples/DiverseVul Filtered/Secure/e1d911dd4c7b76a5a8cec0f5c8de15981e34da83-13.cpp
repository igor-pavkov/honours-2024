static void update_extended_entry(struct tree_desc_x *t, struct name_entry *a)
{
	if (t->d.entry.path == a->path) {
		update_tree_entry(&t->d);
	} else {
		/* we have returned this entry early */
		struct tree_desc_skip *skip = xmalloc(sizeof(*skip));
		skip->ptr = a->path;
		skip->prev = t->skip;
		t->skip = skip;
	}
}
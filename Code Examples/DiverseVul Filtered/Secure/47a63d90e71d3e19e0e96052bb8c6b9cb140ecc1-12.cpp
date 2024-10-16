static int rsync_xal_store(item_list *xalp)
{
	struct ht_int64_node *node;
	int ndx = rsync_xal_l.count; /* pre-incremented count */
	rsync_xa_list *new_list = EXPAND_ITEM_LIST(&rsync_xal_l, rsync_xa_list, RSYNC_XAL_LIST_INITIAL);
	rsync_xa_list_ref *new_ref;
	/* Since the following call starts a new list, we know it will hold the
	 * entire initial-count, not just enough space for one new item. */
	*new_list = empty_xa_list;
	(void)EXPAND_ITEM_LIST(&new_list->xa_items, rsync_xa, xalp->count);
	memcpy(new_list->xa_items.items, xalp->items, xalp->count * sizeof (rsync_xa));
	new_list->xa_items.count = xalp->count;
	xalp->count = 0;

	new_list->ndx = ndx;
	new_list->key = xattr_lookup_hash(&new_list->xa_items);

	if (rsync_xal_h == NULL)
		rsync_xal_h = hashtable_create(512, 1);
	if (rsync_xal_h == NULL)
		out_of_memory("rsync_xal_h hashtable_create()");

	node = hashtable_find(rsync_xal_h, new_list->key, 1);
	if (node == NULL)
		out_of_memory("rsync_xal_h hashtable_find()");

	new_ref = new0(rsync_xa_list_ref);
	if (new_ref == NULL)
		out_of_memory("new0(rsync_xa_list_ref)");

	new_ref->ndx = ndx;

	if (node->data != NULL) {
		rsync_xa_list_ref *ref = node->data;

		while (ref != NULL) {
			if (ref->next != NULL) {
				ref = ref->next;
				continue;
			}

			ref->next = new_ref;
			break;
		}
	} else
		node->data = new_ref;

	return ndx;
}
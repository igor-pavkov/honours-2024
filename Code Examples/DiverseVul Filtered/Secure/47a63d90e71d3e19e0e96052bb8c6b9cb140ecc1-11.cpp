void uncache_tmp_xattrs(void)
{
	if (prior_xattr_count != (size_t)-1) {
		rsync_xa_list *xa_list_item = rsync_xal_l.items;
		rsync_xa_list *xa_list_start = xa_list_item + prior_xattr_count;
		xa_list_item += rsync_xal_l.count;
		rsync_xal_l.count = prior_xattr_count;
		while (xa_list_item-- > xa_list_start) {
			struct ht_int64_node *node;
			rsync_xa_list_ref *ref;

			rsync_xal_free(&xa_list_item->xa_items);

			if (rsync_xal_h == NULL)
				continue;

			node = hashtable_find(rsync_xal_h, xa_list_item->key, 0);
			if (node == NULL)
				continue;

			if (node->data == NULL)
				continue;

			ref = node->data;
			if (xa_list_item->ndx == ref->ndx) {
				/* xa_list_item is the first in the list. */
				node->data = ref->next;
				free(ref);
				continue;
			}

			while (ref != NULL) {
				if (ref->next == NULL) {
					ref = NULL;
					break;
				}
				if (xa_list_item->ndx == ref->next->ndx) {
					ref->next = ref->next->next;
					free(ref);
					break;
				}
				ref = ref->next;
			}
		}
		prior_xattr_count = (size_t)-1;
	}
}
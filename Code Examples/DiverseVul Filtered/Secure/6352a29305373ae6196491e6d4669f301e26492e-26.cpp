static void wipe_auth_tok_list(struct list_head *auth_tok_list_head)
{
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item;
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item_tmp;

	list_for_each_entry_safe(auth_tok_list_item, auth_tok_list_item_tmp,
				 auth_tok_list_head, list) {
		list_del(&auth_tok_list_item->list);
		kmem_cache_free(ecryptfs_auth_tok_list_item_cache,
				auth_tok_list_item);
	}
}
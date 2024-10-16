static void nf_tables_trans_destroy_work(struct work_struct *w)
{
	struct nft_trans *trans, *next;
	LIST_HEAD(head);

	spin_lock(&nf_tables_destroy_list_lock);
	list_splice_init(&nf_tables_destroy_list, &head);
	spin_unlock(&nf_tables_destroy_list_lock);

	if (list_empty(&head))
		return;

	synchronize_rcu();

	list_for_each_entry_safe(trans, next, &head, list) {
		list_del(&trans->list);
		nft_commit_release(trans);
	}
}
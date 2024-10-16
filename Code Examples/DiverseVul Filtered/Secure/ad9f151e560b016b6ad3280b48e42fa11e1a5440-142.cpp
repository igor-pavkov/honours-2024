static void nf_tables_commit_release(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_trans *trans;

	/* all side effects have to be made visible.
	 * For example, if a chain named 'foo' has been deleted, a
	 * new transaction must not find it anymore.
	 *
	 * Memory reclaim happens asynchronously from work queue
	 * to prevent expensive synchronize_rcu() in commit phase.
	 */
	if (list_empty(&nft_net->commit_list)) {
		nf_tables_module_autoload_cleanup(net);
		mutex_unlock(&nft_net->commit_mutex);
		return;
	}

	trans = list_last_entry(&nft_net->commit_list,
				struct nft_trans, list);
	get_net(trans->ctx.net);
	WARN_ON_ONCE(trans->put_net);

	trans->put_net = true;
	spin_lock(&nf_tables_destroy_list_lock);
	list_splice_tail_init(&nft_net->commit_list, &nf_tables_destroy_list);
	spin_unlock(&nf_tables_destroy_list_lock);

	nf_tables_module_autoload_cleanup(net);
	schedule_work(&trans_destroy_work);

	mutex_unlock(&nft_net->commit_mutex);
}
static void nf_tables_module_autoload_cleanup(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_module_request *req, *next;

	WARN_ON_ONCE(!list_empty(&nft_net->commit_list));
	list_for_each_entry_safe(req, next, &nft_net->module_list, list) {
		WARN_ON_ONCE(!req->done);
		list_del(&req->list);
		kfree(req);
	}
}
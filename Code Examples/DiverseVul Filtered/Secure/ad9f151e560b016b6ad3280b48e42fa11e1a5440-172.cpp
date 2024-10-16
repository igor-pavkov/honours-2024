static void nf_tables_module_autoload(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_module_request *req, *next;
	LIST_HEAD(module_list);

	list_splice_init(&nft_net->module_list, &module_list);
	mutex_unlock(&nft_net->commit_mutex);
	list_for_each_entry_safe(req, next, &module_list, list) {
		request_module("%s", req->module);
		req->done = true;
	}
	mutex_lock(&nft_net->commit_mutex);
	list_splice(&module_list, &nft_net->module_list);
}
static bool nf_tables_valid_genid(struct net *net, u32 genid)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	bool genid_ok;

	mutex_lock(&nft_net->commit_mutex);

	genid_ok = genid == 0 || nft_net->base_seq == genid;
	if (!genid_ok)
		mutex_unlock(&nft_net->commit_mutex);

	/* else, commit mutex has to be released by commit or abort function */
	return genid_ok;
}
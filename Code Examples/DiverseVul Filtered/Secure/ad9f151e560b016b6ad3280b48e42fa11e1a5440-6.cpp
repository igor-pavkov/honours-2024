static bool lockdep_commit_lock_is_held(const struct net *net)
{
#ifdef CONFIG_PROVE_LOCKING
	struct nftables_pernet *nft_net = nft_pernet(net);

	return lockdep_is_held(&nft_net->commit_mutex);
#else
	return true;
#endif
}
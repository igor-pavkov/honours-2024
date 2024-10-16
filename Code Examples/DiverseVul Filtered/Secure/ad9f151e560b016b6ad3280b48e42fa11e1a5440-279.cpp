static __be16 nft_base_seq(const struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);

	return htons(nft_net->base_seq & 0xffff);
}
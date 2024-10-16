static void nf_tables_cleanup(struct net *net)
{
	nft_validate_state_update(net, NFT_VALIDATE_SKIP);
}
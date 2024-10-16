void nft_register_chain_type(const struct nft_chain_type *ctype)
{
	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	if (WARN_ON(__nft_chain_type_get(ctype->family, ctype->type))) {
		nfnl_unlock(NFNL_SUBSYS_NFTABLES);
		return;
	}
	chain_type[ctype->family][ctype->type] = ctype;
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
}
void nf_tables_deactivate_flowtable(const struct nft_ctx *ctx,
				    struct nft_flowtable *flowtable,
				    enum nft_trans_phase phase)
{
	switch (phase) {
	case NFT_TRANS_PREPARE:
	case NFT_TRANS_ABORT:
	case NFT_TRANS_RELEASE:
		flowtable->use--;
		fallthrough;
	default:
		return;
	}
}
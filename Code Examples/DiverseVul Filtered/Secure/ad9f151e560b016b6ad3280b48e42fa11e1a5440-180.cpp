static void nft_validate_state_update(struct net *net, u8 new_validate_state)
{
	struct nftables_pernet *nft_net = nft_pernet(net);

	switch (nft_net->validate_state) {
	case NFT_VALIDATE_SKIP:
		WARN_ON_ONCE(new_validate_state == NFT_VALIDATE_DO);
		break;
	case NFT_VALIDATE_NEED:
		break;
	case NFT_VALIDATE_DO:
		if (new_validate_state == NFT_VALIDATE_NEED)
			return;
	}

	nft_net->validate_state = new_validate_state;
}
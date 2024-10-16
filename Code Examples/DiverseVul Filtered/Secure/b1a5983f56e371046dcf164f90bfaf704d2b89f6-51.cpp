static void nft_fwd_neigh_eval(const struct nft_expr *expr,
			      struct nft_regs *regs,
			      const struct nft_pktinfo *pkt)
{
	struct nft_fwd_neigh *priv = nft_expr_priv(expr);
	void *addr = &regs->data[priv->sreg_addr];
	int oif = regs->data[priv->sreg_dev];
	unsigned int verdict = NF_STOLEN;
	struct sk_buff *skb = pkt->skb;
	struct net_device *dev;
	int neigh_table;

	switch (priv->nfproto) {
	case NFPROTO_IPV4: {
		struct iphdr *iph;

		if (skb->protocol != htons(ETH_P_IP)) {
			verdict = NFT_BREAK;
			goto out;
		}
		if (skb_try_make_writable(skb, sizeof(*iph))) {
			verdict = NF_DROP;
			goto out;
		}
		iph = ip_hdr(skb);
		ip_decrease_ttl(iph);
		neigh_table = NEIGH_ARP_TABLE;
		break;
		}
	case NFPROTO_IPV6: {
		struct ipv6hdr *ip6h;

		if (skb->protocol != htons(ETH_P_IPV6)) {
			verdict = NFT_BREAK;
			goto out;
		}
		if (skb_try_make_writable(skb, sizeof(*ip6h))) {
			verdict = NF_DROP;
			goto out;
		}
		ip6h = ipv6_hdr(skb);
		ip6h->hop_limit--;
		neigh_table = NEIGH_ND_TABLE;
		break;
		}
	default:
		verdict = NFT_BREAK;
		goto out;
	}

	dev = dev_get_by_index_rcu(nft_net(pkt), oif);
	if (dev == NULL)
		return;

	skb->dev = dev;
	skb->tstamp = 0;
	neigh_xmit(neigh_table, dev, addr, skb);
out:
	regs->verdict.code = verdict;
}
static void nft_commit_notify(struct net *net, u32 portid)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct sk_buff *batch_skb = NULL, *nskb, *skb;
	unsigned char *data;
	int len;

	list_for_each_entry_safe(skb, nskb, &nft_net->notify_list, list) {
		if (!batch_skb) {
new_batch:
			batch_skb = skb;
			len = NLMSG_GOODSIZE - skb->len;
			list_del(&skb->list);
			continue;
		}
		len -= skb->len;
		if (len > 0 && NFT_CB(skb).report == NFT_CB(batch_skb).report) {
			data = skb_put(batch_skb, skb->len);
			memcpy(data, skb->data, skb->len);
			list_del(&skb->list);
			kfree_skb(skb);
			continue;
		}
		nfnetlink_send(batch_skb, net, portid, NFNLGRP_NFTABLES,
			       NFT_CB(batch_skb).report, GFP_KERNEL);
		goto new_batch;
	}

	if (batch_skb) {
		nfnetlink_send(batch_skb, net, portid, NFNLGRP_NFTABLES,
			       NFT_CB(batch_skb).report, GFP_KERNEL);
	}

	WARN_ON_ONCE(!list_empty(&nft_net->notify_list));
}
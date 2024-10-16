static void nft_notify_enqueue(struct sk_buff *skb, bool report,
			       struct list_head *notify_list)
{
	NFT_CB(skb).report = report;
	list_add_tail(&skb->list, notify_list);
}
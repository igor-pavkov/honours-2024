static int nf_tables_getgen(struct sk_buff *skb, const struct nfnl_info *info,
			    const struct nlattr * const nla[])
{
	struct sk_buff *skb2;
	int err;

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_ATOMIC);
	if (skb2 == NULL)
		return -ENOMEM;

	err = nf_tables_fill_gen_info(skb2, info->net, NETLINK_CB(skb).portid,
				      info->nlh->nlmsg_seq);
	if (err < 0)
		goto err_fill_gen_info;

	return nfnetlink_unicast(skb2, info->net, NETLINK_CB(skb).portid);

err_fill_gen_info:
	kfree_skb(skb2);
	return err;
}
static int nf_tables_fill_flowtable_info(struct sk_buff *skb, struct net *net,
					 u32 portid, u32 seq, int event,
					 u32 flags, int family,
					 struct nft_flowtable *flowtable,
					 struct list_head *hook_list)
{
	struct nlattr *nest, *nest_devs;
	struct nft_hook *hook;
	struct nlmsghdr *nlh;

	event = nfnl_msg_type(NFNL_SUBSYS_NFTABLES, event);
	nlh = nfnl_msg_put(skb, portid, seq, event, flags, family,
			   NFNETLINK_V0, nft_base_seq(net));
	if (!nlh)
		goto nla_put_failure;

	if (nla_put_string(skb, NFTA_FLOWTABLE_TABLE, flowtable->table->name) ||
	    nla_put_string(skb, NFTA_FLOWTABLE_NAME, flowtable->name) ||
	    nla_put_be32(skb, NFTA_FLOWTABLE_USE, htonl(flowtable->use)) ||
	    nla_put_be64(skb, NFTA_FLOWTABLE_HANDLE, cpu_to_be64(flowtable->handle),
			 NFTA_FLOWTABLE_PAD) ||
	    nla_put_be32(skb, NFTA_FLOWTABLE_FLAGS, htonl(flowtable->data.flags)))
		goto nla_put_failure;

	nest = nla_nest_start_noflag(skb, NFTA_FLOWTABLE_HOOK);
	if (!nest)
		goto nla_put_failure;
	if (nla_put_be32(skb, NFTA_FLOWTABLE_HOOK_NUM, htonl(flowtable->hooknum)) ||
	    nla_put_be32(skb, NFTA_FLOWTABLE_HOOK_PRIORITY, htonl(flowtable->data.priority)))
		goto nla_put_failure;

	nest_devs = nla_nest_start_noflag(skb, NFTA_FLOWTABLE_HOOK_DEVS);
	if (!nest_devs)
		goto nla_put_failure;

	list_for_each_entry_rcu(hook, hook_list, list) {
		if (nla_put_string(skb, NFTA_DEVICE_NAME, hook->ops.dev->name))
			goto nla_put_failure;
	}
	nla_nest_end(skb, nest_devs);
	nla_nest_end(skb, nest);

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_trim(skb, nlh);
	return -1;
}
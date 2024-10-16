static int nf_tables_fill_gen_info(struct sk_buff *skb, struct net *net,
				   u32 portid, u32 seq)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nlmsghdr *nlh;
	char buf[TASK_COMM_LEN];
	int event = nfnl_msg_type(NFNL_SUBSYS_NFTABLES, NFT_MSG_NEWGEN);

	nlh = nfnl_msg_put(skb, portid, seq, event, 0, AF_UNSPEC,
			   NFNETLINK_V0, nft_base_seq(net));
	if (!nlh)
		goto nla_put_failure;

	if (nla_put_be32(skb, NFTA_GEN_ID, htonl(nft_net->base_seq)) ||
	    nla_put_be32(skb, NFTA_GEN_PROC_PID, htonl(task_pid_nr(current))) ||
	    nla_put_string(skb, NFTA_GEN_PROC_NAME, get_task_comm(buf, current)))
		goto nla_put_failure;

	nlmsg_end(skb, nlh);
	return 0;

nla_put_failure:
	nlmsg_trim(skb, nlh);
	return -EMSGSIZE;
}
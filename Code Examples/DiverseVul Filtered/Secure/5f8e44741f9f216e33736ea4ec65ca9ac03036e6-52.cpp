static int nlmsg_populate_fdb(struct sk_buff *skb,
			      struct netlink_callback *cb,
			      struct net_device *dev,
			      int *idx,
			      struct netdev_hw_addr_list *list)
{
	struct netdev_hw_addr *ha;
	int err;
	u32 portid, seq;

	portid = NETLINK_CB(cb->skb).portid;
	seq = cb->nlh->nlmsg_seq;

	list_for_each_entry(ha, &list->list, list) {
		if (*idx < cb->args[0])
			goto skip;

		err = nlmsg_populate_fdb_fill(skb, dev, ha->addr, 0,
					      portid, seq,
					      RTM_NEWNEIGH, NTF_SELF,
					      NLM_F_MULTI, NUD_PERMANENT);
		if (err < 0)
			return err;
skip:
		*idx += 1;
	}
	return 0;
}
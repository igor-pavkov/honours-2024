static int nft_netlink_dump_start_rcu(struct sock *nlsk, struct sk_buff *skb,
				      const struct nlmsghdr *nlh,
				      struct netlink_dump_control *c)
{
	int err;

	if (!try_module_get(THIS_MODULE))
		return -EINVAL;

	rcu_read_unlock();
	err = netlink_dump_start(nlsk, skb, nlh, c);
	rcu_read_lock();
	module_put(THIS_MODULE);

	return err;
}
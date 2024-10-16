static int ip_vs_genl_dump_daemons(struct sk_buff *skb,
				   struct netlink_callback *cb)
{
	struct net *net = skb_sknet(skb);
	struct netns_ipvs *ipvs = net_ipvs(net);

	mutex_lock(&ipvs->sync_mutex);
	if ((ipvs->sync_state & IP_VS_STATE_MASTER) && !cb->args[0]) {
		if (ip_vs_genl_dump_daemon(skb, IP_VS_STATE_MASTER,
					   ipvs->master_mcast_ifn,
					   ipvs->master_syncid, cb) < 0)
			goto nla_put_failure;

		cb->args[0] = 1;
	}

	if ((ipvs->sync_state & IP_VS_STATE_BACKUP) && !cb->args[1]) {
		if (ip_vs_genl_dump_daemon(skb, IP_VS_STATE_BACKUP,
					   ipvs->backup_mcast_ifn,
					   ipvs->backup_syncid, cb) < 0)
			goto nla_put_failure;

		cb->args[1] = 1;
	}

nla_put_failure:
	mutex_unlock(&ipvs->sync_mutex);

	return skb->len;
}
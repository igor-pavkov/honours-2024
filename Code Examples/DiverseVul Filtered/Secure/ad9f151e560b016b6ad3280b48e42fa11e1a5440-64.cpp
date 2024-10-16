static int nf_tables_flowtable_event(struct notifier_block *this,
				     unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct nft_flowtable *flowtable;
	struct nftables_pernet *nft_net;
	struct nft_table *table;
	struct net *net;

	if (event != NETDEV_UNREGISTER)
		return 0;

	net = dev_net(dev);
	nft_net = nft_pernet(net);
	mutex_lock(&nft_net->commit_mutex);
	list_for_each_entry(table, &nft_net->tables, list) {
		list_for_each_entry(flowtable, &table->flowtables, list) {
			nft_flowtable_event(event, dev, flowtable);
		}
	}
	mutex_unlock(&nft_net->commit_mutex);

	return NOTIFY_DONE;
}
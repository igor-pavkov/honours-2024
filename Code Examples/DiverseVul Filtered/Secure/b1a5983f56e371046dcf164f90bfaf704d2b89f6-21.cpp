int nft_offload_init(void)
{
	return register_netdevice_notifier(&nft_offload_netdev_notifier);
}
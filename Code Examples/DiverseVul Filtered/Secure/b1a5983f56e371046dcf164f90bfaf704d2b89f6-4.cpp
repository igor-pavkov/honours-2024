static void __exit nft_fwd_netdev_module_exit(void)
{
	nft_unregister_expr(&nft_fwd_netdev_type);
}
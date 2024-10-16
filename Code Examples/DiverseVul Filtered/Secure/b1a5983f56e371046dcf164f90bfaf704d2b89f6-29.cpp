static void __exit nft_dup_netdev_module_exit(void)
{
	nft_unregister_expr(&nft_dup_netdev_type);
}
static int __init nft_dup_netdev_module_init(void)
{
	return nft_register_expr(&nft_dup_netdev_type);
}
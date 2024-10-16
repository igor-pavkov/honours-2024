static int __init nft_fwd_netdev_module_init(void)
{
	return nft_register_expr(&nft_fwd_netdev_type);
}
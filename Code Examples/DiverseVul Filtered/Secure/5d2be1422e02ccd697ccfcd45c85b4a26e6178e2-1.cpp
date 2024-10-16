void tipc_netlink_compat_stop(void)
{
	genl_unregister_family(&tipc_genl_compat_family);
}
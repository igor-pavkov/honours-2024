int tipc_netlink_compat_start(void)
{
	int res;

	res = genl_register_family_with_ops(&tipc_genl_compat_family,
					    tipc_genl_compat_ops);
	if (res) {
		pr_err("Failed to register legacy compat interface\n");
		return res;
	}

	return 0;
}
static int ipxcfg_get_config_data(struct ipx_config_data __user *arg)
{
	struct ipx_config_data vals;

	vals.ipxcfg_auto_create_interfaces = ipxcfg_auto_create_interfaces;
	vals.ipxcfg_auto_select_primary	   = ipxcfg_auto_select_primary;

	return copy_to_user(arg, &vals, sizeof(vals)) ? -EFAULT : 0;
}
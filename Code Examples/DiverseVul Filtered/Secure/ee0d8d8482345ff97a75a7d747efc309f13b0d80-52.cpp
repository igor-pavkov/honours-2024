static void ipxcfg_set_auto_select(char val)
{
	ipxcfg_auto_select_primary = val;
	if (val && !ipx_primary_net)
		ipx_primary_net = ipx_interfaces_head();
}
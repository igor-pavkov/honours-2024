static int selinux_peerlbl_enabled(void)
{
	return (selinux_policycap_alwaysnetwork || netlbl_enabled() || selinux_xfrm_enabled());
}
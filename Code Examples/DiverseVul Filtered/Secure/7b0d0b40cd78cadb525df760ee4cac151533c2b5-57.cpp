static int selinux_secmark_enabled(void)
{
	return (selinux_policycap_alwaysnetwork || atomic_read(&selinux_secmark_refcount));
}
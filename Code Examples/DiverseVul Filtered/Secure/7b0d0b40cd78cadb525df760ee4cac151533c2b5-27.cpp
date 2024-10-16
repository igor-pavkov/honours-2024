static void selinux_tun_dev_free_security(void *security)
{
	kfree(security);
}
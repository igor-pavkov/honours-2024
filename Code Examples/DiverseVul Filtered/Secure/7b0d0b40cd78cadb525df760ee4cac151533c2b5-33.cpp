static int selinux_tun_dev_alloc_security(void **security)
{
	struct tun_security_struct *tunsec;

	tunsec = kzalloc(sizeof(*tunsec), GFP_KERNEL);
	if (!tunsec)
		return -ENOMEM;
	tunsec->sid = current_sid();

	*security = tunsec;
	return 0;
}
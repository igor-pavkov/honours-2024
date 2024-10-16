static int selinux_ib_alloc_security(void **ib_sec)
{
	struct ib_security_struct *sec;

	sec = kzalloc(sizeof(*sec), GFP_KERNEL);
	if (!sec)
		return -ENOMEM;
	sec->sid = current_sid();

	*ib_sec = sec;
	return 0;
}
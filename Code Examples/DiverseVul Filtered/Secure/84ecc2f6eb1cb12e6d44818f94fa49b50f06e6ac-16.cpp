int con_set_trans_old(unsigned char __user * arg)
{
	int i;
	unsigned short inbuf[E_TABSZ];
	unsigned char ubuf[E_TABSZ];

	if (copy_from_user(ubuf, arg, E_TABSZ))
		return -EFAULT;

	for (i = 0; i < E_TABSZ ; i++)
		inbuf[i] = UNI_DIRECT_BASE | ubuf[i];

	console_lock();
	memcpy(translations[USER_MAP], inbuf, sizeof(inbuf));
	update_user_maps();
	console_unlock();
	return 0;
}
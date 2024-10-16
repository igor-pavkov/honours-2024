int con_set_trans_new(ushort __user * arg)
{
	unsigned short inbuf[E_TABSZ];

	if (copy_from_user(inbuf, arg, sizeof(inbuf)))
		return -EFAULT;

	console_lock();
	memcpy(translations[USER_MAP], inbuf, sizeof(inbuf));
	update_user_maps();
	console_unlock();
	return 0;
}
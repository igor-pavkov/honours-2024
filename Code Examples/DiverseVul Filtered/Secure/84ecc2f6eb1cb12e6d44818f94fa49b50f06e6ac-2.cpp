int con_get_trans_new(ushort __user * arg)
{
	unsigned short outbuf[E_TABSZ];

	console_lock();
	memcpy(outbuf, translations[USER_MAP], sizeof(outbuf));
	console_unlock();

	return copy_to_user(arg, outbuf, sizeof(outbuf)) ? -EFAULT : 0;
}
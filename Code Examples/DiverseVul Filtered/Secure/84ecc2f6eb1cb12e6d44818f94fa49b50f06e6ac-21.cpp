int con_get_trans_old(unsigned char __user * arg)
{
	int i, ch;
	unsigned short *p = translations[USER_MAP];
	unsigned char outbuf[E_TABSZ];

	console_lock();
	for (i = 0; i < E_TABSZ ; i++)
	{
		ch = conv_uni_to_pc(vc_cons[fg_console].d, p[i]);
		outbuf[i] = (ch & ~0xff) ? 0 : ch;
	}
	console_unlock();

	return copy_to_user(arg, outbuf, sizeof(outbuf)) ? -EFAULT : 0;
}
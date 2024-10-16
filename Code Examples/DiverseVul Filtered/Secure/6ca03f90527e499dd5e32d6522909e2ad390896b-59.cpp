static void k_spec(struct vc_data *vc, unsigned char value, char up_flag)
{
	if (up_flag)
		return;
	if (value >= ARRAY_SIZE(fn_handler))
		return;
	if ((kbd->kbdmode == VC_RAW ||
	     kbd->kbdmode == VC_MEDIUMRAW ||
	     kbd->kbdmode == VC_OFF) &&
	     value != KVAL(K_SAK))
		return;		/* SAK is allowed even in raw mode */
	fn_handler[value](vc);
}
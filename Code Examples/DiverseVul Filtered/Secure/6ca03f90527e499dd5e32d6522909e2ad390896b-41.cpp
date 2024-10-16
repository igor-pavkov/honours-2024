static void k_unicode(struct vc_data *vc, unsigned int value, char up_flag)
{
	if (up_flag)
		return;		/* no action, if this is a key release */

	if (diacr)
		value = handle_diacr(vc, value);

	if (dead_key_next) {
		dead_key_next = false;
		diacr = value;
		return;
	}
	if (kbd->kbdmode == VC_UNICODE)
		to_utf8(vc, value);
	else {
		int c = conv_uni_to_8bit(value);
		if (c != -1)
			put_queue(vc, c);
	}
}
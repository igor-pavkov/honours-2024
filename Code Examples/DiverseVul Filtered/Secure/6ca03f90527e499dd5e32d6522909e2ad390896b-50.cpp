static void k_deadunicode(struct vc_data *vc, unsigned int value, char up_flag)
{
	if (up_flag)
		return;

	diacr = (diacr ? handle_diacr(vc, value) : value);
}
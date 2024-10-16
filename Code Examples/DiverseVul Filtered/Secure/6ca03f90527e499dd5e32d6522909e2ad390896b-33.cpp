static void k_cons(struct vc_data *vc, unsigned char value, char up_flag)
{
	if (up_flag)
		return;

	set_console(value);
}
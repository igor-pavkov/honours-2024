static void lbs_unset_basic_rate_flags(u8 *rates, size_t len)
{
	int i;

	for (i = 0; i < len; i++)
		rates[i] &= 0x7f;
}
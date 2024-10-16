static void k_self(struct vc_data *vc, unsigned char value, char up_flag)
{
	k_unicode(vc, conv_8bit_to_uni(value), up_flag);
}
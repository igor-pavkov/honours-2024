void gf_net_set_ntp_shift(s32 shift)
{
	ntp_shift = GF_NTP_SEC_1900_TO_1970 + shift;
}
int lbs_ssid_cmp(uint8_t *ssid1, uint8_t ssid1_len, uint8_t *ssid2,
		 uint8_t ssid2_len)
{
	if (ssid1_len != ssid2_len)
		return -1;

	return memcmp(ssid1, ssid2, ssid1_len);
}
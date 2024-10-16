static uint32_t get_company_id(const uint8_t cid[3])
{
	return cid[0] << 16 | cid[1] << 8 | cid[2];
}
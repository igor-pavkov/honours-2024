static void set_company_id(uint8_t cid[3], uint32_t cid_in)
{
	cid[0] = (cid_in & 0xff0000) >> 16;
	cid[1] = (cid_in & 0x00ff00) >> 8;
	cid[2] = (cid_in & 0x0000ff);
}
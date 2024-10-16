static u32 lsr_read_codec_IDREF_command(GF_LASeRCodec *lsr, const char *name)
{
	u32 flag;
	u32 nID = 1+lsr_read_vluimsbf5(lsr, name);

	GF_LSR_READ_INT(lsr, flag, 1, "reserved");
	if (flag) {
		u32 len = lsr_read_vluimsbf5(lsr, "len");
		GF_LSR_READ_INT(lsr, flag, len, "reserved");
	}
	return nID;
}
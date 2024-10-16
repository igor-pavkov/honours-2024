static GF_Err lsr_read_save(GF_LASeRCodec *lsr, GF_List *com_list)
{
	u32 i, count;
	count = lsr_read_vluimsbf5(lsr, "nbIds");
	for (i=0; i<count; i++) {
		u32 flag;
		lsr_read_vluimsbf5(lsr, "ref[[i]]");
		GF_LSR_READ_INT(lsr, flag, 1, "reserved");

		lsr_get_attribute_name(lsr);
		if (lsr->last_error) return lsr->last_error;
	}
	lsr_read_byte_align_string(lsr, NULL, "groupID");
	lsr_read_any_attribute(lsr, NULL, 1);
	return GF_OK;
}
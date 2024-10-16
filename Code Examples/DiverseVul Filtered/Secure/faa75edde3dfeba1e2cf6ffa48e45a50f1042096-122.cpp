static GF_Err lsr_read_restore(GF_LASeRCodec *lsr, GF_List *com_list)
{
	lsr_read_byte_align_string(lsr, NULL, "groupID");
	lsr_read_any_attribute(lsr, NULL, 1);
	return GF_OK;
}
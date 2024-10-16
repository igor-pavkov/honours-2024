GF_Err gf_isom_set_byte_offset(GF_ISOFile *file, s64 byte_offset)
{
	if (!file) return GF_BAD_PARAM;
	file->read_byte_offset = byte_offset;
	return GF_OK;
}
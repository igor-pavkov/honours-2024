GF_Err uuid_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 bytesToRead;
	GF_UnknownUUIDBox *ptr = (GF_UnknownUUIDBox *)s;
	if (ptr->size > 0xFFFFFFFF) return GF_ISOM_INVALID_FILE;
	bytesToRead = (u32) (ptr->size);

	if (bytesToRead) {
		ptr->data = (char*)gf_malloc(bytesToRead);
		if (ptr->data == NULL ) return GF_OUT_OF_MEM;
		ptr->dataSize = bytesToRead;
		gf_bs_read_data(bs, ptr->data, ptr->dataSize);
	}
	return GF_OK;
}
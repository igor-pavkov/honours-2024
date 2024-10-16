GF_Err payt_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_PAYTBox *ptr = (GF_PAYTBox *)s;

	ISOM_DECREASE_SIZE(ptr, 5 );
	ptr->payloadCode = gf_bs_read_u32(bs);
	length = gf_bs_read_u8(bs);
	ISOM_DECREASE_SIZE(ptr, length);
	ptr->payloadString = (char*)gf_malloc(sizeof(char) * (length+1) );
	if (! ptr->payloadString) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->payloadString, length);
	ptr->payloadString[length] = 0;

	return GF_OK;
}
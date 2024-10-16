GF_Err iSLT_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ISMACrypSaltBox *ptr = (GF_ISMACrypSaltBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->salt = gf_bs_read_u64(bs);
	return GF_OK;
}
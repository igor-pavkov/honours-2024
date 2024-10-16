GF_Err iKMS_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 len;
	GF_ISMAKMSBox *ptr = (GF_ISMAKMSBox *)s;

	len = (u32) (ptr->size);
	ptr->URI = (char*) gf_malloc(sizeof(char)*len);
	if (!ptr->URI) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->URI, len);
	return GF_OK;
}
GF_Err padb_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_PaddingBitsBox *ptr = (GF_PaddingBitsBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->SampleCount = gf_bs_read_u32(bs);
	if (ptr->size < ptr->SampleCount/2) //half byte per sample
		return GF_ISOM_INVALID_FILE;

	ptr->padbits = (u8 *)gf_malloc(sizeof(u8)*ptr->SampleCount);
	if (!ptr->padbits) return GF_OUT_OF_MEM;

	for (i=0; i<ptr->SampleCount; i += 2) {
		gf_bs_read_int(bs, 1);
		if (i+1 < ptr->SampleCount) {
			ptr->padbits[i+1] = gf_bs_read_int(bs, 3);
		} else {
			gf_bs_read_int(bs, 3);
		}
		gf_bs_read_int(bs, 1);
		ptr->padbits[i] = gf_bs_read_int(bs, 3);
	}
	return GF_OK;
}
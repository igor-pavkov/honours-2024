GF_Err tfdt_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_TFBaseMediaDecodeTimeBox *ptr = (GF_TFBaseMediaDecodeTimeBox *)s;

	if (ptr->version==1) {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->baseMediaDecodeTime = gf_bs_read_u64(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->baseMediaDecodeTime = (u32) gf_bs_read_u32(bs);
	}
	return GF_OK;
}
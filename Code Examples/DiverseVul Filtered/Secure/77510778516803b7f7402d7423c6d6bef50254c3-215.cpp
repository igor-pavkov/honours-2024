GF_Err pdin_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox*)s;

	ptr->count = (u32) (ptr->size) / 8;
	ptr->rates = (u32*)gf_malloc(sizeof(u32)*ptr->count);
	if (!ptr->rates) return GF_OUT_OF_MEM;
	ptr->times = (u32*)gf_malloc(sizeof(u32)*ptr->count);
	if (!ptr->times) return GF_OUT_OF_MEM;
	for (i=0; i<ptr->count; i++) {
		ptr->rates[i] = gf_bs_read_u32(bs);
		ptr->times[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}
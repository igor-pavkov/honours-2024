GF_Err pdin_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	for (i=0; i<ptr->count; i++) {
		gf_bs_write_u32(bs, ptr->rates[i]);
		gf_bs_write_u32(bs, ptr->times[i]);
	}
	return GF_OK;
}
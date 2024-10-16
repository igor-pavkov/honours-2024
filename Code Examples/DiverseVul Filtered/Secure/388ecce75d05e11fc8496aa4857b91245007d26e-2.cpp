GF_Err stss_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->nb_entries);
	for (i = 0; i < ptr->nb_entries; i++) {
		gf_bs_write_u32(bs, ptr->sampleNumbers[i]);
	}
	return GF_OK;
}
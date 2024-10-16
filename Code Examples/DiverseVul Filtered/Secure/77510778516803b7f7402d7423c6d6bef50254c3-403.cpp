GF_Err subs_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i, j, entry_count;
	u16 subsample_count;
	GF_SubSampleEntry *pSubSamp;
	GF_SubSampleInformationBox *ptr = (GF_SubSampleInformationBox *) s;

	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	entry_count = gf_list_count(ptr->Samples);
	gf_bs_write_u32(bs, entry_count);

	for (i=0; i<entry_count; i++) {
		GF_SubSampleInfoEntry *pSamp = (GF_SubSampleInfoEntry*) gf_list_get(ptr->Samples, i);
		subsample_count = gf_list_count(pSamp->SubSamples);
		gf_bs_write_u32(bs, pSamp->sample_delta);
		gf_bs_write_u16(bs, subsample_count);

		for (j=0; j<subsample_count; j++) {
			pSubSamp = (GF_SubSampleEntry*) gf_list_get(pSamp->SubSamples, j);
			if (ptr->version == 1) {
				gf_bs_write_u32(bs, pSubSamp->subsample_size);
			} else {
				gf_bs_write_u16(bs, pSubSamp->subsample_size);
			}
			gf_bs_write_u8(bs, pSubSamp->subsample_priority);
			gf_bs_write_u8(bs, pSubSamp->discardable);
			gf_bs_write_u32(bs, pSubSamp->reserved);
		}
	}
	return e;
}
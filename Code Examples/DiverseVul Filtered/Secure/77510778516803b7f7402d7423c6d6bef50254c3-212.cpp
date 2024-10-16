GF_Err subs_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SubSampleInformationBox *ptr = (GF_SubSampleInformationBox *)s;
	u32 entry_count, i, j;
	u16 subsample_count;

	ISOM_DECREASE_SIZE(ptr, 4);
	entry_count = gf_bs_read_u32(bs);

	for (i=0; i<entry_count; i++) {
		u32 subs_size=0;
		GF_SubSampleInfoEntry *pSamp = (GF_SubSampleInfoEntry*) gf_malloc(sizeof(GF_SubSampleInfoEntry));
		if (!pSamp) return GF_OUT_OF_MEM;

		memset(pSamp, 0, sizeof(GF_SubSampleInfoEntry));
		pSamp->SubSamples = gf_list_new();
		pSamp->sample_delta = gf_bs_read_u32(bs);
		subsample_count = gf_bs_read_u16(bs);
		subs_size=6;

		for (j=0; j<subsample_count; j++) {
			GF_SubSampleEntry *pSubSamp = (GF_SubSampleEntry*) gf_malloc(sizeof(GF_SubSampleEntry));
			if (!pSubSamp) return GF_OUT_OF_MEM;

			memset(pSubSamp, 0, sizeof(GF_SubSampleEntry));
			if (ptr->version==1) {
				pSubSamp->subsample_size = gf_bs_read_u32(bs);
				subs_size+=4;
			} else {
				pSubSamp->subsample_size = gf_bs_read_u16(bs);
				subs_size+=2;
			}
			pSubSamp->subsample_priority = gf_bs_read_u8(bs);
			pSubSamp->discardable = gf_bs_read_u8(bs);
			pSubSamp->reserved = gf_bs_read_u32(bs);
			subs_size+=6;

			gf_list_add(pSamp->SubSamples, pSubSamp);
		}
		gf_list_add(ptr->Samples, pSamp);
		ISOM_DECREASE_SIZE(ptr, subs_size);
	}
	return GF_OK;
}
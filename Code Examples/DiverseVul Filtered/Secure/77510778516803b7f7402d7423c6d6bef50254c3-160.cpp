void subs_box_del(GF_Box *s)
{
	GF_SubSampleInformationBox *ptr = (GF_SubSampleInformationBox *)s;
	if (ptr == NULL) return;

	while (gf_list_count(ptr->Samples)) {
		GF_SubSampleInfoEntry *pSamp;
		pSamp = (GF_SubSampleInfoEntry*)gf_list_get(ptr->Samples, 0);
		while (gf_list_count(pSamp->SubSamples)) {
			GF_SubSampleEntry *pSubSamp;
			pSubSamp = (GF_SubSampleEntry*) gf_list_get(pSamp->SubSamples, 0);
			gf_free(pSubSamp);
			gf_list_rem(pSamp->SubSamples, 0);
		}
		gf_list_del(pSamp->SubSamples);
		gf_free(pSamp);
		gf_list_rem(ptr->Samples, 0);
	}
	gf_list_del(ptr->Samples);
	gf_free(ptr);
}
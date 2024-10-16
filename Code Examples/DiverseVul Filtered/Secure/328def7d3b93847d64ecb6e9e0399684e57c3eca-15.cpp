GF_ISOMDataRefAllType Media_SelfContainedType(GF_MediaBox *mdia)
{
	u32 nb_ext, nb_self;
	u32 i, count;

	nb_ext = nb_self = 0;
	count = mdia->information->sampleTable->SampleDescription ? gf_list_count(mdia->information->sampleTable->SampleDescription->child_boxes) : 0;
	for (i=0; i<count; i++) {
		if (Media_IsSelfContained(mdia, i+1)) nb_self++;
		else nb_ext++;
	}
	if (nb_ext==count) return ISOM_DREF_EXT;
	if (nb_self==count) return ISOM_DREF_SELF;
	return ISOM_DREF_MIXED;
}
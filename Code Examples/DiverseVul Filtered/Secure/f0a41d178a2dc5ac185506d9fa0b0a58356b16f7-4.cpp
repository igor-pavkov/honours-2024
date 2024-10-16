GF_Err gf_isom_sample_get_subsample(GF_ISOFile *movie, u32 track, u32 sampleNumber, u32 flags, u32 subSampleNumber, u32 *size, u8 *priority, u32 *reserved, Bool *discardable)
{
	GF_SubSampleEntry *entry;
	GF_SubSampleInfoEntry *sub_sample;
	u32 count = gf_isom_sample_get_subsample_entry(movie, track, sampleNumber, flags, &sub_sample);
	if (!size || !priority || !discardable) return GF_BAD_PARAM;

	if (!subSampleNumber || (subSampleNumber>count)) return GF_BAD_PARAM;
	entry = (GF_SubSampleEntry*)gf_list_get(sub_sample->SubSamples, subSampleNumber-1);
	*size = entry->subsample_size;
	*priority = entry->subsample_priority;
	*reserved = entry->reserved;
	*discardable = entry->discardable ? GF_TRUE : GF_FALSE;
	return GF_OK;
}
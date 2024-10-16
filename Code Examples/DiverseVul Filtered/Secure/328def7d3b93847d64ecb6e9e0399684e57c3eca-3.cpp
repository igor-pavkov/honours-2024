GF_Err Media_GetSampleDesc(GF_MediaBox *mdia, u32 SampleDescIndex, GF_SampleEntryBox **out_entry, u32 *dataRefIndex)
{
	GF_SampleDescriptionBox *stsd;
	GF_SampleEntryBox *entry = NULL;

	if (!mdia) return GF_ISOM_INVALID_FILE;

	stsd = mdia->information->sampleTable->SampleDescription;
	if (!stsd) return GF_ISOM_INVALID_FILE;
	if (!SampleDescIndex || (SampleDescIndex > gf_list_count(stsd->child_boxes)) ) return GF_BAD_PARAM;

	entry = (GF_SampleEntryBox*)gf_list_get(stsd->child_boxes, SampleDescIndex - 1);
	if (!entry) return GF_ISOM_INVALID_FILE;

	if (out_entry) *out_entry = entry;
	if (dataRefIndex) *dataRefIndex = entry->dataReferenceIndex;
	return GF_OK;
}
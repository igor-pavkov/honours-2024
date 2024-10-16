GF_Err Media_GetSampleDescIndex(GF_MediaBox *mdia, u64 DTS, u32 *sampleDescIndex)
{
	GF_Err e;
	u32 sampleNumber, prevSampleNumber, num;
	u64 offset;
	if (sampleDescIndex == NULL) return GF_BAD_PARAM;

	//find the sample for this time
	e = stbl_findEntryForTime(mdia->information->sampleTable, (u32) DTS, 0, &sampleNumber, &prevSampleNumber);
	if (e) return e;

	if (!sampleNumber && !prevSampleNumber) {
		//we have to assume the track was created to be used... If we have a sampleDesc, OK
		if (gf_list_count(mdia->information->sampleTable->SampleDescription->child_boxes)) {
			(*sampleDescIndex) = 1;
			return GF_OK;
		}
		return GF_BAD_PARAM;
	}
	return stbl_GetSampleInfos(mdia->information->sampleTable, ( sampleNumber ? sampleNumber : prevSampleNumber), &offset, &num, sampleDescIndex, NULL);
}
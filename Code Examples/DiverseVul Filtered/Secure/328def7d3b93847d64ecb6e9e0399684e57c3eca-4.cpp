GF_Err Media_FindSyncSample(GF_SampleTableBox *stbl, u32 searchFromSample, u32 *sampleNumber, u8 mode)
{
	GF_ISOSAPType isRAP;
	u32 next, prev, next_in_sap, prev_in_sap;
	if (!stbl || !stbl->SyncSample) return GF_BAD_PARAM;

	//set to current sample if we don't find a RAP
	*sampleNumber = searchFromSample;

	//this is not the exact sample, but the prev move to next sample if enough samples....
	if ( (mode == GF_ISOM_SEARCH_SYNC_FORWARD) && (searchFromSample == stbl->SampleSize->sampleCount) ) {
		return GF_OK;
	}
	if ( (mode == GF_ISOM_SEARCH_SYNC_BACKWARD) && !searchFromSample) {
		*sampleNumber = 1;
		return GF_OK;
	}
	//get the entry
	stbl_GetSampleRAP(stbl->SyncSample, searchFromSample, &isRAP, &prev, &next);
	if (isRAP) {
		(*sampleNumber) = searchFromSample;
		return GF_OK;
	}

	/*check sample groups - prev & next are overwritten if RAP group is found, but are not re-initialized otherwise*/
	stbl_SearchSAPs(stbl, searchFromSample, &isRAP, &prev_in_sap, &next_in_sap);
	if (isRAP) {
		(*sampleNumber) = searchFromSample;
		return GF_OK;
	}

	if (prev_in_sap > prev)
		prev = prev_in_sap;
	if (next_in_sap && next_in_sap < next)
		next = next_in_sap;

	//nothing yet, go for next time...
	if (mode == GF_ISOM_SEARCH_SYNC_FORWARD) {
		if (next) *sampleNumber = next;
	} else {
		if (prev) *sampleNumber = prev;
	}

	return GF_OK;
}
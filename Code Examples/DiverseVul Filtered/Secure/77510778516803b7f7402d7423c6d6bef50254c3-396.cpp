GF_Err reftype_AddRefTrack(GF_TrackReferenceTypeBox *ref, GF_ISOTrackID trackID, u16 *outRefIndex)
{
	u32 i;
	if (!ref || !trackID) return GF_BAD_PARAM;

	if (outRefIndex) *outRefIndex = 0;
	//don't add a dep if already here !!
	for (i = 0; i < ref->trackIDCount; i++) {
		if (ref->trackIDs[i] == trackID) {
			if (outRefIndex) *outRefIndex = i+1;
			return GF_OK;
		}
	}

	ref->trackIDs = (GF_ISOTrackID *) gf_realloc(ref->trackIDs, (ref->trackIDCount + 1) * sizeof(GF_ISOTrackID) );
	if (!ref->trackIDs) return GF_OUT_OF_MEM;
	ref->trackIDs[ref->trackIDCount] = trackID;
	ref->trackIDCount++;
	if (outRefIndex) *outRefIndex = ref->trackIDCount;
	return GF_OK;
}
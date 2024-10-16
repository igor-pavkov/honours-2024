GF_Err Media_UpdateSampleReference(GF_MediaBox *mdia, u32 sampleNumber, GF_ISOSample *sample, u64 data_offset)
{
	GF_Err e;
	u32 drefIndex, chunkNum, descIndex;
	u64 off, DTS;
	GF_DataEntryURLBox *Dentry;
	GF_SampleTableBox *stbl;

	if (!mdia) return GF_BAD_PARAM;
	stbl = mdia->information->sampleTable;

	//check we have the sampe dts
	e = stbl_GetSampleDTS(stbl->TimeToSample, sampleNumber, &DTS);
	if (e) return e;
	if (DTS != sample->DTS) return GF_BAD_PARAM;

	//get our infos
	stbl_GetSampleInfos(stbl, sampleNumber, &off, &chunkNum, &descIndex, NULL);

	//then check the data ref
	e = Media_GetSampleDesc(mdia, descIndex, NULL, &drefIndex);
	if (e) return e;
	Dentry = (GF_DataEntryURLBox*)gf_list_get(mdia->information->dataInformation->dref->child_boxes, drefIndex - 1);
	if (!Dentry) return GF_ISOM_INVALID_FILE;

	//we only modify self-contained data
	if (Dentry->flags == 1) return GF_ISOM_INVALID_MODE;

	//and we don't modify the media data
	return UpdateSample(mdia, sampleNumber, sample->dataLength, sample->CTS_Offset, data_offset, sample->IsRAP);
}
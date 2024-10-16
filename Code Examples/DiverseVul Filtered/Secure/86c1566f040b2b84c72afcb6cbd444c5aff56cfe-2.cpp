GF_Err gf_isom_end_hint_sample(GF_ISOFile *the_file, u32 trackNumber, u8 IsRandomAccessPoint)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	u32 dataRefIndex;
	GF_Err e;
	GF_BitStream *bs;
	GF_ISOSample *samp;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !IsHintTrack(trak)) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, trak->Media->information->sampleTable->currentEntryIndex, (GF_SampleEntryBox **) &entry, &dataRefIndex);
	if (e) return e;
	if (!entry->hint_sample) return GF_BAD_PARAM;

	//first of all, we need to adjust the offset for data referenced IN THIS hint sample
	//and get some PckSize
	e = AdjustHintInfo(entry, trak->Media->information->sampleTable->SampleSize->sampleCount + 1);
	if (e) return e;

	//ok, let's write the sample
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	e = gf_isom_hint_sample_write(entry->hint_sample, bs);
	if (e) {
		gf_bs_del(bs);
		return e;
	}
	samp = gf_isom_sample_new();
	samp->CTS_Offset = 0;
	samp->IsRAP = IsRandomAccessPoint;
	samp->DTS = entry->hint_sample->TransmissionTime;
	//get the sample
	gf_bs_get_content(bs, &samp->data, &samp->dataLength);
	gf_bs_del(bs);

	//finally add the sample
	e = gf_isom_add_sample(the_file, trackNumber, trak->Media->information->sampleTable->currentEntryIndex, samp);
	gf_isom_sample_del(&samp);

	//and delete the sample in our entry ...
	gf_isom_hint_sample_del(entry->hint_sample);
	entry->hint_sample = NULL;
	return e;
}
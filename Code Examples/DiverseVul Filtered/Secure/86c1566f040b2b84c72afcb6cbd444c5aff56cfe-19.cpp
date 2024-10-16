GF_Err gf_isom_new_hint_description(GF_ISOFile *the_file, u32 trackNumber, s32 HintTrackVersion, s32 LastCompatibleVersion, u8 Rely, u32 *HintDescriptionIndex)
{
	GF_Err e;
	u32 drefIndex;
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *hdesc;
	GF_SampleDescriptionBox *stsd;
	GF_RelyHintBox *relyA;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	*HintDescriptionIndex = 0;
	if (!trak || !IsHintTrack(trak)) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	//OK, create a new HintSampleDesc
	hdesc = (GF_HintSampleEntryBox *) gf_isom_box_new_parent(&stsd->child_boxes, GetHintFormat(trak));
	if (!hdesc) return GF_OUT_OF_MEM;

	if (HintTrackVersion > 0) hdesc->HintTrackVersion = HintTrackVersion;
	if (LastCompatibleVersion > 0) hdesc->LastCompatibleVersion = LastCompatibleVersion;

	//create a data reference - WE ONLY DEAL WITH SELF-CONTAINED HINT TRACKS
	e = Media_CreateDataRef(the_file, trak->Media->information->dataInformation->dref, NULL, NULL, &drefIndex);
	if (e) return e;
	hdesc->dataReferenceIndex = drefIndex;

	*HintDescriptionIndex = gf_list_count(stsd->child_boxes);

	//RTP needs a default timeScale... use the media one.
	if (CheckHintFormat(trak, GF_ISOM_HINT_RTP)) {
		e = gf_isom_rtp_set_timescale(the_file, trackNumber, *HintDescriptionIndex, trak->Media->mediaHeader->timeScale);
		if (e) return e;
	}
	if (!Rely) return GF_OK;

	//we need a rely box (common to all protocols)
	relyA = (GF_RelyHintBox *) gf_isom_box_new_parent(&hdesc->child_boxes, GF_ISOM_BOX_TYPE_RELY);
	if (!relyA) return GF_OUT_OF_MEM;
	if (Rely == 1) {
		relyA->preferred = 1;
	} else {
		relyA->required = 1;
	}
	return GF_OK;
}
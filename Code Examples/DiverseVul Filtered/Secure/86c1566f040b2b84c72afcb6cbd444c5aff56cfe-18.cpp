GF_Err gf_isom_rtp_set_timescale(GF_ISOFile *the_file, u32 trackNumber, u32 HintDescriptionIndex, u32 TimeScale)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *hdesc;
	u32 i, count;
	GF_TSHintEntryBox *ent;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return GF_BAD_PARAM;

	//OK, create a new HintSampleDesc
	hdesc = (GF_HintSampleEntryBox *)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, HintDescriptionIndex - 1);
	if (!hdesc) return GF_BAD_PARAM;

	count = gf_list_count(hdesc->child_boxes);

	for (i=0; i< count; i++) {
		ent = (GF_TSHintEntryBox *)gf_list_get(hdesc->child_boxes, i);
		if (ent->type == GF_ISOM_BOX_TYPE_TIMS) {
			ent->timeScale = TimeScale;
			return GF_OK;
		}
	}
	//we have to create a new entry...
	ent = (GF_TSHintEntryBox *) gf_isom_box_new_parent(&hdesc->child_boxes, GF_ISOM_BOX_TYPE_TIMS);
	if (!ent) return GF_OUT_OF_MEM;
	ent->timeScale = TimeScale;
	return GF_OK;
}
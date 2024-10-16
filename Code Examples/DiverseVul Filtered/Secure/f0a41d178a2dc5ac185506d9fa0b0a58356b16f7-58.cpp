GF_Err gf_isom_get_audio_layout(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, GF_AudioChannelLayout *layout)
{
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;
	GF_ChannelLayoutBox *chnl;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !layout) return GF_BAD_PARAM;
	memset(layout, 0, sizeof(GF_AudioChannelLayout));

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) return movie->LastError = GF_BAD_PARAM;

	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_AUDIO) return GF_BAD_PARAM;
	chnl = (GF_ChannelLayoutBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_CHNL);
	if (!chnl) return GF_NOT_FOUND;

	memcpy(layout, &chnl->layout, sizeof(GF_AudioChannelLayout));
	return GF_OK;
}
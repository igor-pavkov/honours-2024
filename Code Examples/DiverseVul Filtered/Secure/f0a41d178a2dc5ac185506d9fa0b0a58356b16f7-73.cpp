GF_Err gf_isom_get_clean_aperture(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 *cleanApertureWidthN, u32 *cleanApertureWidthD, u32 *cleanApertureHeightN, u32 *cleanApertureHeightD, s32 *horizOffN, u32 *horizOffD, s32 *vertOffN, u32 *vertOffD)
{
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	if (entry == NULL) return GF_BAD_PARAM;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	GF_CleanApertureBox *clap = (GF_CleanApertureBox *)gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_CLAP);

	if (cleanApertureWidthN) *cleanApertureWidthN = clap ? clap->cleanApertureWidthN : 0;
	if (cleanApertureWidthD) *cleanApertureWidthD = clap ? clap->cleanApertureWidthD : 0;
	if (cleanApertureHeightN) *cleanApertureHeightN = clap ? clap->cleanApertureHeightN : 0;
	if (cleanApertureHeightD) *cleanApertureHeightD = clap ? clap->cleanApertureHeightD : 0;
	if (horizOffN) *horizOffN = clap ? clap->horizOffN : 0;
	if (horizOffD) *horizOffD = clap ? clap->horizOffD : 0;
	if (vertOffN) *vertOffN = clap ? clap->vertOffN : 0;
	if (vertOffD) *vertOffD = clap ? clap->vertOffD : 0;
	return GF_OK;
}
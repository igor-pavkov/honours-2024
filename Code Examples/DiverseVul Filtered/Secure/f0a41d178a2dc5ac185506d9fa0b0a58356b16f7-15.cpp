GF_Err gf_isom_get_color_info(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 *colour_type, u16 *colour_primaries, u16 *transfer_characteristics, u16 *matrix_coefficients, Bool *full_range_flag)
{
	GF_TrackBox *trak;
	GF_VisualSampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) return movie->LastError = GF_BAD_PARAM;

	entry = (GF_VisualSampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_OK;

	//valid for MPEG visual, JPG and 3GPP H263
	if (entry->internal_type!=GF_ISOM_SAMPLE_ENTRY_VIDEO) {
		return GF_BAD_PARAM;
	}
	GF_ColourInformationBox *clr = (GF_ColourInformationBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_COLR);
	if (!clr) return GF_NOT_FOUND;

	if (colour_type) *colour_type = clr->colour_type;
	if (colour_primaries) *colour_primaries = clr->colour_primaries;
	if (transfer_characteristics) *transfer_characteristics = clr->transfer_characteristics;
	if (matrix_coefficients) *matrix_coefficients = clr->matrix_coefficients;
	if (full_range_flag) *full_range_flag = clr->full_range_flag;
	return GF_OK;
}
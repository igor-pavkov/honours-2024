GF_Err gf_isom_get_pixel_aspect_ratio(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 *hSpacing, u32 *vSpacing)
{
	GF_TrackBox *trak;
	GF_VisualSampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !hSpacing || !vSpacing) return GF_BAD_PARAM;
	*hSpacing = 1;
	*vSpacing = 1;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) return movie->LastError = GF_BAD_PARAM;

	entry = (GF_VisualSampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_OK;

	//valid for MPEG visual, JPG and 3GPP H263
	if (entry->internal_type==GF_ISOM_SAMPLE_ENTRY_VIDEO) {
		GF_PixelAspectRatioBox *pasp = (GF_PixelAspectRatioBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_PASP);
		if (pasp) {
			*hSpacing = pasp->hSpacing;
			*vSpacing = pasp->vSpacing;
		}
		return GF_OK;
	} else {
		return GF_BAD_PARAM;
	}
}
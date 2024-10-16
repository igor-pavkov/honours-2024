GF_Err gf_isom_get_original_format_type(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex, u32 *outOriginalFormat)
{
	GF_TrackBox *trak;
	GF_SampleEntryBox *sea;
	GF_ProtectionSchemeInfoBox *sinf;
	u32 i, count;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	count = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
	for (i=0; i<count; i++) {
		if (sampleDescriptionIndex && (i+1 != sampleDescriptionIndex)) continue;

		Media_GetSampleDesc(trak->Media, i+1, &sea, NULL);
		if (!sea) return GF_BAD_PARAM;
		sinf = (GF_ProtectionSchemeInfoBox*) gf_isom_box_find_child(sea->child_boxes, GF_ISOM_BOX_TYPE_SINF);
		if (!sinf) continue;

		if (outOriginalFormat && sinf->original_format) {
			*outOriginalFormat = sinf->original_format->data_format;
		}
		return GF_OK;
	}
	if (outOriginalFormat) *outOriginalFormat = 0;
	return GF_OK;
}
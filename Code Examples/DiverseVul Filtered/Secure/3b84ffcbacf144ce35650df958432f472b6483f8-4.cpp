GF_Err gf_isom_remove_track_protection(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_Err e;
	GF_SampleEntryBox *sea;
	GF_ProtectionSchemeInfoBox *sinf;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !sampleDescriptionIndex) return GF_BAD_PARAM;

	sea = NULL;
	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CENC_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CBC_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CENS_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_CBCS_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_ISMACRYP_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_OMADRM_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_ADOBE_SCHEME, &sea);
	if (!sinf) sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_PIFF_SCHEME, &sea);
	if (!sinf) return GF_OK;

	sea->type = sinf->original_format->data_format;
	while (1) {
		GF_Box *b = gf_isom_box_find_child(sea->child_boxes, GF_ISOM_BOX_TYPE_SINF);
		if (!b) break;
		gf_isom_box_del_parent(&sea->child_boxes, b);
	}
	if (sea->type == GF_ISOM_BOX_TYPE_264B) sea->type = GF_ISOM_BOX_TYPE_AVC1;
	if (sea->type == GF_ISOM_BOX_TYPE_265B) sea->type = GF_ISOM_BOX_TYPE_HVC1;
	return GF_OK;
}
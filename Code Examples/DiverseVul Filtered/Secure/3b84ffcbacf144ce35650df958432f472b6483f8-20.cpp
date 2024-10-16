Bool gf_isom_is_cenc_media(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_ProtectionSchemeInfoBox *sinf;
	u32 i, count;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;

	count = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
	for (i=0; i<count; i++) {
		if (sampleDescriptionIndex && (i+1 != sampleDescriptionIndex)) continue;

		sinf = isom_get_sinf_entry(trak, i+1, GF_ISOM_CENC_SCHEME, NULL);
		if (!sinf) sinf = isom_get_sinf_entry(trak, i+1, GF_ISOM_CBC_SCHEME, NULL);
		if (!sinf) sinf = isom_get_sinf_entry(trak, i+1, GF_ISOM_CENS_SCHEME, NULL);
		if (!sinf) sinf = isom_get_sinf_entry(trak, i+1, GF_ISOM_CBCS_SCHEME, NULL);
		if (!sinf) sinf = isom_get_sinf_entry(trak, i+1, GF_ISOM_PIFF_SCHEME, NULL);

		if (!sinf) continue;

		/*non-encrypted or non-CENC*/
		if (!sinf->scheme_type)
			return GF_FALSE;

		switch (sinf->scheme_type->scheme_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case GF_ISOM_SVE1_SCHEME:
			return GF_TRUE;
		default:
			return GF_FALSE;
		}
		return GF_TRUE;
	}
	return GF_FALSE;

}
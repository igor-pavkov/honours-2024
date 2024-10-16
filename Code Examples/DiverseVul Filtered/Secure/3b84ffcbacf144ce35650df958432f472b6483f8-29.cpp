GF_Err gf_isom_change_ismacryp_protection(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex, char *scheme_uri, char *kms_uri)
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
	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_ISMACRYP_SCHEME, &sea);
	if (!sinf) return GF_OK;

	if (scheme_uri) {
		gf_free(sinf->scheme_type->URI);
		sinf->scheme_type->URI = gf_strdup(scheme_uri);
	}
	if (kms_uri) {
		gf_free(sinf->info->ikms->URI);
		sinf->info->ikms->URI = gf_strdup(kms_uri);
	}
	return GF_OK;
}
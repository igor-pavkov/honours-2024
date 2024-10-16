Bool gf_isom_is_omadrm_media(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_ProtectionSchemeInfoBox *sinf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;

	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_OMADRM_SCHEME, NULL);
	if (!sinf) return GF_FALSE;

	/*non-encrypted or non-OMA*/
	if (!sinf->info || !sinf->info->odkm || !sinf->info->odkm->hdr)
		return GF_FALSE;

	return GF_TRUE;
}
static GF_ProtectionSchemeInfoBox *isom_get_sinf_entry(GF_TrackBox *trak, u32 sampleDescriptionIndex, u32 scheme_type, GF_SampleEntryBox **out_sea)
{
	u32 i=0;
	GF_SampleEntryBox *sea=NULL;
	GF_ProtectionSchemeInfoBox *sinf;

	Media_GetSampleDesc(trak->Media, sampleDescriptionIndex, &sea, NULL);
	if (!sea) return NULL;

	i = 0;
	while ((sinf = (GF_ProtectionSchemeInfoBox*)gf_list_enum(sea->child_boxes, &i))) {
		if (sinf->type != GF_ISOM_BOX_TYPE_SINF) continue;

		if (sinf->original_format && sinf->scheme_type && sinf->info) {
			if (!scheme_type || (sinf->scheme_type->scheme_type == scheme_type)) {
				if (out_sea)
					*out_sea = sea;
				return sinf;
			}
		}
	}
	return NULL;
}
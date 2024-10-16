Bool gf_isom_needs_layer_reconstruction(GF_ISOFile *file)
{
	u32 count, i;
	if (!file)
		return GF_FALSE;
	count = gf_isom_get_track_count(file);
	for (i = 0; i < count; i++) {
		if (gf_isom_get_reference_count(file, i+1, GF_ISOM_REF_SCAL) > 0) {
			return GF_TRUE;
		}
		if (gf_isom_get_reference_count(file, i+1, GF_ISOM_REF_SABT) > 0) {
			return GF_TRUE;
		}
		switch (gf_isom_get_media_subtype(file, i+1, 1)) {
		case GF_ISOM_SUBTYPE_LHV1:
		case GF_ISOM_SUBTYPE_LHE1:
		case GF_ISOM_SUBTYPE_HVC2:
		case GF_ISOM_SUBTYPE_HEV2:
			if (gf_isom_get_reference_count(file, i+1, GF_ISOM_REF_BASE) > 0) {
				return GF_TRUE;
			}
		}
	}
	return GF_FALSE;
}
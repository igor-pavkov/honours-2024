GF_Err Track_FindRef(GF_TrackBox *trak, u32 ReferenceType, GF_TrackReferenceTypeBox **dpnd)
{
	GF_TrackReferenceBox *ref;
	GF_TrackReferenceTypeBox *a;
	u32 i;
	if (! trak) return GF_BAD_PARAM;
	if (! trak->References) {
		*dpnd = NULL;
		return GF_OK;
	}
	ref = trak->References;
	i=0;
	while ((a = (GF_TrackReferenceTypeBox *)gf_list_enum(ref->child_boxes, &i))) {
		if (a->reference_type == ReferenceType) {
			*dpnd = a;
			return GF_OK;
		}
	}
	*dpnd = NULL;
	return GF_OK;
}
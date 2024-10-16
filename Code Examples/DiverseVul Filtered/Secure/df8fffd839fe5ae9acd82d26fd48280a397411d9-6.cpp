GF_Err Track_RemoveRef(GF_TrackBox *trak, u32 ReferenceType)
{
	GF_TrackReferenceBox *ref;
	GF_Box *a;
	u32 i;
	if (! trak) return GF_BAD_PARAM;
	if (! trak->References) return GF_OK;
	ref = trak->References;
	i=0;
	while ((a = (GF_Box *)gf_list_enum(ref->child_boxes, &i))) {
		if (a->type == ReferenceType) {
			gf_isom_box_del_parent(&ref->child_boxes, a);
			return GF_OK;
		}
	}
	return GF_OK;
}
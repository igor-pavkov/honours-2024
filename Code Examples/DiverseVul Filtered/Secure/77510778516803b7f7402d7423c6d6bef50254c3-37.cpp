GF_Err strk_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_SubTrackBox *ptr = (GF_SubTrackBox *)s;
	if (!a) return GF_OK;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_STRI:
		BOX_FIELD_ASSIGN(info, GF_SubTrackInformationBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_STRD:
		BOX_FIELD_ASSIGN(strd, GF_Box)
		return GF_OK;
	}
	return GF_OK;
}
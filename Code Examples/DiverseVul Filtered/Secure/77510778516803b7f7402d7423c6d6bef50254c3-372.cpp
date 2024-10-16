GF_Err fdsa_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_HintSample *ptr = (GF_HintSample *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_FDPA:
		BOX_FIELD_LIST_ASSIGN(packetTable)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_EXTR:
		BOX_FIELD_ASSIGN(extra_data, GF_ExtraDataBox)
		break;
	}
	return GF_OK;
}
GF_Err mvex_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *)s;

	switch (a->type) {
	case GF_ISOM_BOX_TYPE_TREX:
		return gf_list_add(ptr->TrackExList, a);
	case GF_ISOM_BOX_TYPE_TREP:
		return gf_list_add(ptr->TrackExPropList, a);
	case GF_ISOM_BOX_TYPE_MEHD:
		if (ptr->mehd) ERROR_ON_DUPLICATED_BOX(a, s)
		ptr->mehd = (GF_MovieExtendsHeaderBox*)a;
		return GF_OK;
	}
	return GF_OK;
}
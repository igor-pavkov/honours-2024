GF_Err srpp_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_SRTPProcessBox *ptr = (GF_SRTPProcessBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_SCHI:
		if (ptr->info) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->info = (GF_SchemeInformationBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SCHM:
		if (ptr->scheme_type) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->scheme_type = (GF_SchemeTypeBox *)a;
		return GF_OK;
	}
	return GF_OK;
}
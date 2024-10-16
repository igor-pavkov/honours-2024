GF_Err dinf_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_DataInformationBox *ptr = (GF_DataInformationBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_DREF:
		if (ptr->dref) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->dref = (GF_DataReferenceBox *)a;
		return GF_OK;
	}
	return GF_OK;
}
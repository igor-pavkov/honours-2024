GF_Err edts_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_EditBox *ptr = (GF_EditBox *)s;
	if (a->type == GF_ISOM_BOX_TYPE_ELST) {
		if (ptr->editList) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->editList = (GF_EditListBox *)a;
		return GF_OK;
	} else {
		return GF_OK;
	}
	return GF_OK;
}
GF_Err mfra_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MovieFragmentRandomAccessBox *ptr = (GF_MovieFragmentRandomAccessBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_TFRA:
		return gf_list_add(ptr->tfra_list, a);
	case GF_ISOM_BOX_TYPE_MFRO:
		if (ptr->mfro) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->mfro = (GF_MovieFragmentRandomAccessOffsetBox *)a;
		return GF_OK;
	}
	return GF_OK;
}
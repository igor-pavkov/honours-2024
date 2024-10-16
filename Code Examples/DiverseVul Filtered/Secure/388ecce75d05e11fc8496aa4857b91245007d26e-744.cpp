GF_Err moof_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MovieFragmentBox *ptr = (GF_MovieFragmentBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_MFHD:
		if (ptr->mfhd) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->mfhd = (GF_MovieFragmentHeaderBox *) a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TRAF:
		return gf_list_add(ptr->TrackList, a);
	case GF_ISOM_BOX_TYPE_PSSH:
		if (!ptr->PSSHs) ptr->PSSHs = gf_list_new();
		return gf_list_add(ptr->PSSHs, a);
	}
	return GF_OK;
}
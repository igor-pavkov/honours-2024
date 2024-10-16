GF_Err trgr_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *)s;

	BOX_FIELD_LIST_ASSIGN(groups)
	return gf_list_add(ptr->groups, a);
}
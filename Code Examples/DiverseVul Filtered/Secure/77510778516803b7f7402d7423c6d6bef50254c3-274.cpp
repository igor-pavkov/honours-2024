GF_Err xtra_box_size(GF_Box *s)
{
	GF_XtraBox *ptr = (GF_XtraBox *)s;
	u32 i, count = gf_list_count(ptr->tags);
	for (i=0; i<count; i++) {
		GF_XtraTag *tag = gf_list_get(ptr->tags, i);
		ptr->size += 18 + (u32) strlen(tag->name) + tag->prop_size;
	}
	return GF_OK;
}
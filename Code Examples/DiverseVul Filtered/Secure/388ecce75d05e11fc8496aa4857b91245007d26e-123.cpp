void mvex_box_del(GF_Box *s)
{
	GF_MovieExtendsBox *ptr = (GF_MovieExtendsBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->TrackExList);
	gf_list_del(ptr->TrackExPropList);
	gf_free(ptr);
}